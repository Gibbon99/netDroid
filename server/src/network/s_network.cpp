#include <iostream>
#include <utility>
#include <SDL_image.h>
#include "../../hdr/network/s_network.h"
#include "../../hdr/system/s_peers.h"
#include "../../hdr/main.h"
#include "../../hdr/system/s_shutdown.h"
#include "../../hdr/system/s_resources.h"

#define EVENT_NETWORK_THREAD_NAME   "eventNetworkThread"
#define EVENT_NETWORK_MUTEX_NAME    "eventNetworkMutex"

std::queue<droidEventNetwork> networkEventsQueue{};

//----------------------------------------------------------------------------------------------------------------------
//
// Process a data packet received from a client
void s_processClientPacket (ENetPacket *newDataPacket, size_t dataSize)
//----------------------------------------------------------------------------------------------------------------------
{
	dataPacket dataPacketIn = {};

	pods::InputBuffer                              packetIn (reinterpret_cast<const char *>(newDataPacket->data), dataSize);
	pods::MsgPackDeserializer<decltype (packetIn)> deserializer (packetIn);

	if (deserializer.load (dataPacketIn) != pods::Error::NoError)
	{
		serverMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_LOGFILE, sys_getString ("Error deserializering data packet."));
		return;
	}

	auto fromClientID = dataPacketIn.clientID;

	switch (dataPacketIn.packetType)
	{
		case DATA_PACKET_TYPES::PACKET_REQUEST_INIT_SCRIPT:

			serverMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_LOGFILE, sys_getString ("Received init script request from client [ %i ].", fromClientID));

			s_sendScriptToClient (dataPacketIn.clientID, dataPacketIn.packetString, serverFileMapping.getfileMappedName ("clientInitScript"), DATA_PACKET_TYPES::PACKET_REQUEST_INIT_SCRIPT);
			break;

		default:
			serverMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_LOGFILE, sys_getString ("Received an unknown packet."));
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// This function is called from the thread engine class after being registered at startup
//
// It processes network events from network clients
int s_processEventNetworkThread ([[maybe_unused]]void *param)
//----------------------------------------------------------------------------------------------------------------------
{
	int newClientID = -1;

	static SDL_mutex *eventNetworkMutex = nullptr;

	//
	// Cache getting the mutex value
	if (nullptr == eventNetworkMutex)
	{
		eventNetworkMutex = serverThreads.findMutex (EVENT_NETWORK_MUTEX_NAME);
		if (nullptr == eventNetworkMutex)
		{
			std::cout << serverThreads.getErrorString ().c_str () << std::endl;
			// TODO Exit with error
		}
	}

	while (serverThreads.canThreadRun (EVENT_NETWORK_THREAD_NAME))
	{
		if (serverThreads.isThreadReady (EVENT_NETWORK_THREAD_NAME))
		{
			SDL_Delay (THREAD_DELAY_MS);

			if (!networkEventsQueue.empty ())   // stuff in the queue to process
			{
				if (serverThreads.lockMutex (eventNetworkMutex))
				{
					auto networkEvent = networkEventsQueue.front ();
					networkEventsQueue.pop ();
					serverThreads.unLockMutex (eventNetworkMutex);

					serverMessage.message(MESSAGE_TARGET_DEBUG, sys_getString("Server received network event [ %s ]", getEventType (networkEvent.networkEvent.type).c_str()));

					switch (networkEvent.networkEvent.type)
					{
						case ENET_EVENT_TYPE_CONNECT:
							serverMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("\nA new client CONNECTED from %s:%u.", getHostnameFromAddress (networkEvent.networkEvent.peer->address).c_str (), networkEvent.networkEvent.peer->address.port));
							newClientID = addNewPeer (networkEvent.networkEvent.peer);

							s_sendNewClientID (networkEvent.networkEvent.peer, newClientID);

//							s_sendMediaToClient (networkEvent.networkEvent.peer, "splash", DATA_PACKET_TYPES::PACKET_IMAGE);
//							s_sendMediaToClient (networkEvent.networkEvent.peer, "scrollBeeps", DATA_PACKET_TYPES::PACKET_AUDIO);
							break;
						case ENET_EVENT_TYPE_RECEIVE:
							serverMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("A packet of length %zu containing %s was received from %s on channel %u.", networkEvent.networkEvent.packet->dataLength, networkEvent.networkEvent.packet->data, networkEvent.networkEvent.peer->data, networkEvent.networkEvent.channelID));
							//
							// Clean up the packet now that we're done using it.
							s_processClientPacket (networkEvent.networkEvent.packet, networkEvent.networkEvent.packet->dataLength);
							enet_packet_destroy (networkEvent.networkEvent.packet);

							break;

						case ENET_EVENT_TYPE_DISCONNECT:
							serverMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("%s disconnected.\n", getHostnameFromAddress (networkEvent.networkEvent.peer->address).c_str ()));
							//
							// Reset the peer's client information
							networkEvent.networkEvent.peer->data = nullptr;
							break;

						case ENET_EVENT_TYPE_NONE:
							break;
					}
				}
			}
		}
	}
	return 1;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Called at startup to register the network processing thread and function
bool s_startEventNetworkThread ()
//----------------------------------------------------------------------------------------------------------------------
{
	if (!serverThreads.registerMutex (EVENT_NETWORK_MUTEX_NAME))
	{
		printf ("%s\n", serverThreads.getErrorString ().c_str ());
		return false;
	}
	else
		printf ("Mutex [ %s ] registered.\n", EVENT_NETWORK_MUTEX_NAME);

	if (!serverThreads.registerThread (s_processEventNetworkThread, EVENT_NETWORK_THREAD_NAME))
	{
		printf ("%s\n", serverThreads.getErrorString ().c_str ());
		return false;
	}
	else
		printf ("Thread [ %s ] registered.\n", EVENT_NETWORK_THREAD_NAME);

	if (!serverThreads.setThreadRunState (true, EVENT_NETWORK_THREAD_NAME))
		std::cout << serverThreads.getErrorString () << std::endl;

	if (!serverThreads.setThreadReadyState (true, EVENT_NETWORK_THREAD_NAME))
		std::cout << serverThreads.getErrorString () << std::endl;

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Add a newly received event from the network to the processing queue
void s_addNetworkEventToQueue (ENetEvent newNetworkEvent)
//----------------------------------------------------------------------------------------------------------------------
{
	static SDL_mutex *lockMutex = nullptr;

	if (nullptr == lockMutex)
	{
		lockMutex = serverThreads.findMutex (EVENT_NETWORK_MUTEX_NAME);
		if (nullptr == lockMutex)
			s_shutdownWithError (sys_getString ("%s\n", serverThreads.getErrorString ().c_str ()));
	}
	//
	// Put the new event onto the network queue
	if (serverThreads.lockMutex (lockMutex))   // Blocks if the mutex is locked by another thread
	{
		networkEventsQueue.emplace (newNetworkEvent);
		serverThreads.unLockMutex (lockMutex);
	}
	else
	{
		serverMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("LOCK FAILED [ %s ]", serverThreads.getErrorString ().c_str ()));
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Send the client it's ID reference on the server
void s_sendNewClientID (ENetPeer *peerInfo, int newClientID)
//----------------------------------------------------------------------------------------------------------------------
{
	dataPacket newPacket;

	newPacket.packetType   = DATA_PACKET_TYPES::PACKET_NEW_CLIENT_ID;
	newPacket.packetData   = newClientID;
	newPacket.packetString = "Client ID Index";
	newPacket.binarySize   = 0;
	newPacket.binaryData.clear ();

	if (!com_sendDataToPeer (peerInfo, newPacket))
	{
		serverMessage.message (MESSAGE_TARGET_STD_OUT, "An error occurred attempting to send clientID to new client.");
		return;
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Send an image over the network
void s_sendMediaToClient (ENetPeer *peerInfo, std::string mediaName, DATA_PACKET_TYPES packetType)
//----------------------------------------------------------------------------------------------------------------------
{
	dataPacket newPacket;

	auto tempBuffer = binaryFiles[mediaName].getBlob ();

	SDL_RWops *rw = SDL_RWFromMem (static_cast<void *>(&tempBuffer[0]), binaryFiles[mediaName].getBlobSize ());
	if (nullptr == rw)
	{
		serverMessage.message (MESSAGE_TARGET_STD_OUT, sys_getString ("Couldn't create RWops [ %s ]", SDL_GetError ()));
		return;
	}

	newPacket.packetType   = packetType;
	newPacket.packetData   = 0;
	newPacket.packetString = std::move (mediaName);
	newPacket.binarySize   = binaryFiles[newPacket.packetString].getBlobSize ();

	for (int i = 0; i != newPacket.binarySize; i++)
	{
		newPacket.binaryData.push_back (tempBuffer[i]);
	}

	serverMessage.message (MESSAGE_TARGET_STD_OUT, sys_getString ("Sent image file [ %s ] size [ %i ]", newPacket.packetString.c_str (), newPacket.binaryData.size ()));

	if (!com_sendDataToPeer (peerInfo, newPacket))
	{
		serverMessage.message (MESSAGE_TARGET_STD_OUT, "An error occurred attempting to send image file to client.");
		return;
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Send a shader to the client
void s_sendShaderToClient (ENetPeer *peerInfo, std::string shaderName)
//----------------------------------------------------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------------------------------------------------
//
// Send a script file to the client
void s_sendScriptToClient (int whichClient, std::string clientName, std::string scriptName, DATA_PACKET_TYPES whichScriptType)
//----------------------------------------------------------------------------------------------------------------------
{
	dataPacket newPacket;
	//
	// Read in the script file
	auto newScriptSection = readTextFile(scriptName);
	if (newScriptSection.empty())
	{
		serverMessage.message(MESSAGE_TARGET_STD_OUT, sys_getString("Unable to load client script file [ %s ]", scriptName.c_str()));
		return; // TODO resubmit and send error to client
	}

	newPacket.packetType   = whichScriptType;
	newPacket.packetData   = whichClient;
	newPacket.packetString = clientName;
	//
	// Copy script contents into packet
	newPacket.binaryData.clear ();
	std::copy(newScriptSection.begin(), newScriptSection.end(), std::back_inserter(newPacket.binaryData));
	newPacket.binarySize = newPacket.binaryData.size();
	newPacket.crc = CRC::Calculate(newPacket.binaryData.data(), newPacket.binaryData.size(), CRC::CRC_32());

	if (!com_sendDataToPeer ( &peers[whichClient].peerInfo, newPacket))
	{
		serverMessage.message (MESSAGE_TARGET_STD_OUT, sys_getString("An error occurred attempting to send script to client [ %i ].", whichClient));
		return;
	}

}
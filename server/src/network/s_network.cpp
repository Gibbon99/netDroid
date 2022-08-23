#include <iostream>
#include <utility>
#include <SDL_image.h>
#include "../../hdr/network/s_network.h"
#include "com_globals.h"
#include "../../hdr/system/s_peers.h"
#include "../../hdr/main.h"
#include "../../hdr/system/s_shutdown.h"
#include "../../hdr/system/s_resources.h"

#define EVENT_NETWORK_THREAD_NAME   "eventNetworkThread"
#define EVENT_NETWORK_MUTEX_NAME    "eventNetworkMutex"


std::queue<droidEventNetwork *> networkEventsQueue{};

//----------------------------------------------------------------------------------------------------------------------
//
// This function is called from the thread engine class after being registered at startup
//
// It processes network events from network clients
int processEventNetwork ([[maybe_unused]]void *param)
//----------------------------------------------------------------------------------------------------------------------
{
	int newClientID = -1;

	droidEventNetwork *networkEvent{};
	static SDL_mutex  *eventNetworkMutex = nullptr;

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
				if (nullptr != eventNetworkMutex)
				{
					serverThreads.lockMutex (eventNetworkMutex);   // Blocks if the mutex is locked by another thread
					networkEvent = networkEventsQueue.front ();
					serverThreads.unLockMutex (eventNetworkMutex);

					switch (networkEvent->networkEvent.type)
					{
						case ENET_EVENT_TYPE_CONNECT:
							printf ("A new client connected from %s:%u.\n", getHostnameFromAddress (networkEvent->networkEvent.peer->address).c_str (), networkEvent->networkEvent.peer->address.port);
							newClientID = addNewPeer (networkEvent->networkEvent.peer);

							serverSendNewClientID (networkEvent->networkEvent.peer, newClientID);

//							serverSendMediaToClient (networkEvent->networkEvent.peer, "splash", DATA_PACKET_TYPES::PACKET_IMAGE);
							serverSendMediaToClient (networkEvent->networkEvent.peer, "scrollBeeps", DATA_PACKET_TYPES::PACKET_AUDIO);
//							networkEvent->networkEvent.peer->data = (void *) "Client information";
							break;
						case ENET_EVENT_TYPE_RECEIVE:
//							printf ("A packet of length %zu containing %s was received from %s on channel %u.\n", networkEvent->networkEvent.packet->dataLength, networkEvent->networkEvent.packet->data, networkEvent->networkEvent.peer->data, networkEvent->networkEvent.channelID);
							/* Clean up the packet now that we're done using it. */
							enet_packet_destroy (networkEvent->networkEvent.packet);

							break;

						case ENET_EVENT_TYPE_DISCONNECT:
							printf ("%s disconnected.\n", getHostnameFromAddress (networkEvent->networkEvent.peer->address).c_str ());
							/* Reset the peer's client information. */
							networkEvent->networkEvent.peer->data = nullptr;
							break;

						case ENET_EVENT_TYPE_NONE:
							break;
					}
					serverThreads.lockMutex (eventNetworkMutex);          // Blocks if the mutex is locked by another thread
					delete (networkEventsQueue.front ());       // Free memory
					networkEventsQueue.pop ();
					serverThreads.unLockMutex (eventNetworkMutex);
				}
			}
		}
	}
	return 1;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Called at startup to register the network processing thread and function
bool startEventNetwork ()
//----------------------------------------------------------------------------------------------------------------------
{
	if (!serverThreads.registerMutex (EVENT_NETWORK_MUTEX_NAME))
	{
		printf ("%s\n", serverThreads.getErrorString ().c_str ());
		return false;
	}
	else
		printf ("Mutex [ %s ] registered.\n", EVENT_NETWORK_MUTEX_NAME);

	if (!serverThreads.registerThread (processEventNetwork, EVENT_NETWORK_THREAD_NAME))
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
void addNetworkEvent (ENetEvent newNetworkEvent)
//----------------------------------------------------------------------------------------------------------------------
{
	static SDL_mutex  *lockMutex = nullptr;
	droidEventNetwork *tempNetworkEvent{};

	if (nullptr == lockMutex)
	{
		lockMutex = serverThreads.findMutex (EVENT_NETWORK_MUTEX_NAME);
		if (nullptr == lockMutex)
			s_shutdownWithError (sys_getString ("%s\n", serverThreads.getErrorString ().c_str ()));
	}

	tempNetworkEvent = new droidEventNetwork (newNetworkEvent);
	//
	// Put the new event onto the logfile queue
	serverThreads.lockMutex (lockMutex);   // Blocks if the mutex is locked by another thread
	networkEventsQueue.push (tempNetworkEvent);
	serverThreads.unLockMutex (lockMutex);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Send the client it's ID reference on the server
void serverSendNewClientID (ENetPeer *peerInfo, int newClientID)
//----------------------------------------------------------------------------------------------------------------------
{
	dataPacket newPacket;

	newPacket.packetType = DATA_PACKET_TYPES::PACKET_NEW_CLIENT_ID;
	newPacket.packetData = newClientID;
	newPacket.testString = "Test string from server.";
	newPacket.binarySize = 0;
	newPacket.binaryData.clear ();

	com_sendDataToPeer (peerInfo, newPacket);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Send an image over the network
void serverSendMediaToClient (ENetPeer *peerInfo, std::string mediaName, DATA_PACKET_TYPES packetType)
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

	newPacket.packetType = packetType;
	newPacket.packetData = 0;
	newPacket.testString = std::move (mediaName);
	newPacket.binarySize = binaryFiles[newPacket.testString].getBlobSize ();

	for (int i = 0; i != newPacket.binarySize; i++)
	{
		newPacket.binaryData.push_back (tempBuffer[i]);
	}

	serverMessage.message (MESSAGE_TARGET_STD_OUT, sys_getString ("Sent image file [ %s ] size [ %i ]", newPacket.testString.c_str (), newPacket.binaryData.size ()));

	com_sendDataToPeer (peerInfo, newPacket);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Send a shader to the client
void s_sendShaderToClient (ENetPeer *peerInfo, std::string shaderName)
//----------------------------------------------------------------------------------------------------------------------
{

}
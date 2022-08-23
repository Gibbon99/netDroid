#include "../../hdr/network/c_network.h"
#include "../../hdr/system/c_graphics.h"
#include "../../hdr/system/c_audio.h"

int networkFetchDelayMS{10};

std::queue<droidEventNetwork> networkEventsQueueClient{};

//----------------------------------------------------------------------------------------------------------------------
//
// This function is called from the thread engine class after being registered at startup
//
// It processes network events sent to the client from the server
int processClientEventNetwork ([[maybe_unused]]void *param)
//----------------------------------------------------------------------------------------------------------------------
{
//	droidEventNetwork *networkEvent{};
	static SDL_mutex  *eventNetworkMutex{nullptr};

	//
	// Cache getting the mutex value
	eventNetworkMutex = clientThreads.findMutex (EVENT_CLIENT_NETWORK_MUTEX_NAME);
	if (nullptr == eventNetworkMutex)
	{
		clientMessage.message (MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_CONSOLE, sys_getString ("%s", clientThreads.getErrorString ().c_str ()));
		// TODO Quit with error
	}

	while (clientThreads.canThreadRun (EVENT_CLIENT_NETWORK_THREAD_NAME))
	{

//		printf("CanThreadRun [ %s ]\n", clientThreads.canThreadRun(EVENT_CLIENT_NETWORK_THREAD_NAME) ? "true" : "false");


		if (clientThreads.isThreadReady (EVENT_CLIENT_NETWORK_THREAD_NAME))
		{

			SDL_Delay (networkFetchDelayMS);    // Increase this value to simulate network latency

			if (!networkEventsQueueClient.empty ())   // Events in the queue to process
			{

				printf ("Number of network events in queue [ %lu ]\n", networkEventsQueueClient.size ());


				if (clientThreads.lockMutex (EVENT_CLIENT_NETWORK_MUTEX_NAME))   // Blocks if the mutex is locked by another thread
				{

					printf ("Inside mutex lock - getting networkEvent from queue front.\n");

					auto networkEvent = networkEventsQueueClient.front ();
					networkEventsQueueClient.pop();

					printf ("Inside mutex lock - Type is [ %i ].\n", networkEvent.networkEvent.type);

					switch (networkEvent.networkEvent.type)
					{
						case ENET_EVENT_TYPE_CONNECT:

							printf ("Got ENET_EVENT_TYPE_CONNECT\n\n");

							clientMessage.message (MESSAGE_TARGET_DEBUG | MESSAGE_TARGET_STD_OUT |
							                       MESSAGE_TARGET_CONSOLE, sys_getString ("Connection to [ %s:%i ] succeeded.", getHostnameFromAddress (networkEvent.networkEvent.peer->address).c_str (), networkEvent.networkEvent.peer->address.port));
							clientNetworkState.setNewState (networkStates::NETWORK_STATE_CONNECT_SUCCESS);
							break;

						case ENET_EVENT_TYPE_DISCONNECT:
							clientMessage.message (MESSAGE_TARGET_DEBUG, sys_getString ("%s disconnected.", getHostnameFromAddress (networkEvent.networkEvent.peer->address).c_str ()));
							/* Reset the peer's client information. */
							networkEvent.networkEvent.peer->data = nullptr;
							break;

						case ENET_EVENT_TYPE_RECEIVE:
							clientMessage.message (MESSAGE_TARGET_DEBUG, sys_getString ("A packet of length %zu containing %s was received from %s on channel %u.", networkEvent.networkEvent.packet->dataLength, static_cast<char *>(networkEvent.networkEvent.peer->data), getHostnameFromAddress (networkEvent.networkEvent.peer->address).c_str (), networkEvent.networkEvent.channelID));
							c_processServerPacket (networkEvent.networkEvent.packet, networkEvent.networkEvent.packet->dataLength);
							//
							// Clean up the packet now that we're done using it.
							enet_packet_destroy (networkEvent.networkEvent.packet);
							break;

						case ENET_EVENT_TYPE_NONE:
							break;
					}
//				SDL_LockMutex (eventNetworkMutex);   // Blocks if the mutex is locked by another thread
//				delete (networkEventsQueue.front ());      // Free memory
//					networkEventsQueue.pop ();
//				SDL_UnlockMutex (eventNetworkMutex);


					clientThreads.unLockMutex (EVENT_CLIENT_NETWORK_MUTEX_NAME);
				}
				else
				{
					printf ("ERROR: Could not lock network event mutex [ %s ]\n\n", EVENT_CLIENT_NETWORK_MUTEX_NAME);
//					exit(-1);
					// TODO - Quit with error - could not lock mutex

				}
			}
		}
	}
	clientMessage.message (MESSAGE_TARGET_STD_OUT, sys_getString ("Thread [ %s ] finished.", EVENT_CLIENT_NETWORK_THREAD_NAME));
	return 1;
}


//----------------------------------------------------------------------------------------------------------------------
//
// Called at startup to register the network processing thread and function
bool c_startNetworkMonitor ()
//----------------------------------------------------------------------------------------------------------------------
{
	if (!clientThreads.registerMutex (EVENT_CLIENT_NETWORK_MUTEX_NAME))
	{
		clientMessage.message (MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, clientThreads.getErrorString ());
		return false;
	}
	else
		clientMessage.message (MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, sys_getString ("Mutex [ %s ] registered.", EVENT_CLIENT_NETWORK_MUTEX_NAME));

	if (!clientThreads.registerThread (processClientEventNetwork, EVENT_CLIENT_NETWORK_THREAD_NAME))
	{
		clientMessage.message (MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, clientThreads.getErrorString ());
		return false;
	}
	else
		clientMessage.message (MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, sys_getString ("Thread [ %s ] registered.", EVENT_CLIENT_NETWORK_THREAD_NAME));

	if (!clientThreads.setThreadRunState (true, EVENT_CLIENT_NETWORK_THREAD_NAME))
	{
		clientMessage.message (MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, clientThreads.getErrorString ());
		return false;
	}

	if (!clientThreads.setThreadReadyState (true, EVENT_CLIENT_NETWORK_THREAD_NAME))
	{
		clientMessage.message (MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, clientThreads.getErrorString ());
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Add a newly received event from the network to the processing queue
void c_addNetworkEvent (ENetEvent newNetworkEvent)
//----------------------------------------------------------------------------------------------------------------------
{
	static SDL_mutex  *lockMutex = nullptr;
	//
	// Cache mutex value
	if (lockMutex == nullptr)
	{
		lockMutex = clientThreads.findMutex (EVENT_CLIENT_NETWORK_MUTEX_NAME);
		if (nullptr == lockMutex)
		{
			clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, clientThreads.getErrorString ());
			// TODO Exit with error
		}
	}

	//
	// Put the new event onto the logfile queue
	if (clientThreads.lockMutex (lockMutex))
	{
		printf ("Got a network event. Pushing it onto the queue.\n\n");

		networkEventsQueueClient.emplace (newNetworkEvent);

		printf ("Added new network event to network queue - size [ %zu ]\n", networkEventsQueueClient.size ());
		printf ("New type is [ %i ]\n", networkEventsQueueClient.front ().networkEvent.type);

		clientThreads.unLockMutex (lockMutex);
	}
	else
	{
		printf ("LOCK FAILED : %s\n\n", clientThreads.getErrorString ().c_str ());
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Process a data packet received from the server
void c_processServerPacket (ENetPacket *newDataPacket, size_t dataSize)
//----------------------------------------------------------------------------------------------------------------------
{
	dataPacket dataPacketIn = {};

	pods::InputBuffer                              packetIn (reinterpret_cast<const char *>(newDataPacket->data), dataSize);
	pods::MsgPackDeserializer<decltype (packetIn)> deserializer (packetIn);

	if (deserializer.load (dataPacketIn) != pods::Error::NoError)
	{
		clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_LOGFILE, sys_getString ("Error deserializering data packet."));
		return;
	}

	switch (dataPacketIn.packetType)
	{
		case DATA_PACKET_TYPES::PACKET_NEW_CLIENT_ID:

			clientMessage.message (MESSAGE_TARGET_DEBUG, sys_getString ("Got ID from server [ %i ].", dataPacketIn.packetData));
			clientMessage.message (MESSAGE_TARGET_DEBUG, sys_getString ("Got string [ %s ] from server.", dataPacketIn.testString.c_str ()));
			updateClientPeerID (dataPacketIn.packetData);
			// TODO - Get client address
//			updateClientPeerUsername(sys_getString("netDroid-%s", getHostnameFromAddress (newDataPacket..  clientNetworkObject.getHostPointer()->address).c_str()));
			break;

		case DATA_PACKET_TYPES::PACKET_NEW_USERNAME:
			break;

		case DATA_PACKET_TYPES::PACKET_IMAGE:
			clientMessage.message (MESSAGE_TARGET_DEBUG, sys_getString ("Got image packet. Name [ %s ] Size [ %i ].", dataPacketIn.testString.c_str (), dataPacketIn.binarySize));
			c_convertPacketToSurface (dataPacketIn);
			break;

		case DATA_PACKET_TYPES::PACKET_AUDIO:
			c_convertPacketToAudio (dataPacketIn);
			break;

		default:
			clientMessage.message (MESSAGE_TARGET_DEBUG, sys_getString ("Unknown data packet type."));
			break;
	}
}

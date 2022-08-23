#pragma once

#include <queue>
#include "classes/com_threadsEngine.h"
#include "com_netEvents.h"

// Called at startup to register the network processing thread and function
bool startEventNetwork();

// Add a newly received event from the network to the processing queue
void addNetworkEvent(ENetEvent newNetworkEvent);

// Send the client it's ID reference on the server
void serverSendNewClientID(ENetPeer *peerInfo, int newClientID);

// Send an image over the network
void serverSendMediaToClient (ENetPeer *peerInfo, std::string mediaName, DATA_PACKET_TYPES packetType);

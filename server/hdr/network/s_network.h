#pragma once

#include <queue>
#include "classes/com_threadsEngine.h"
#include "com_netEvents.h"

// Called at startup to register the network processing thread and function
bool s_startEventNetworkThread();

// Add a newly received event from the network to the processing queue
void s_addNetworkEventToQueue(ENetEvent newNetworkEvent);

// Send the client it's ID reference on the server
void s_sendNewClientID(ENetPeer *peerInfo, int newClientID);

// Send an image over the network
void s_sendMediaToClient (ENetPeer *peerInfo, std::string mediaName, DATA_PACKET_TYPES packetType);

// Send a script file to the client
void s_sendScriptToClient (int whichClient, std::string scriptName, DATA_PACKET_TYPES whichScriptType);

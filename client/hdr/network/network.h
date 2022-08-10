#pragma once

#include "enet/enet.h"
#include "com_globals.h"

// Called at startup to register the network processing thread and function
bool startClientEventNetwork();

// Add a newly received event from the network to the processing queue
void addClientNetworkEvent(ENetEvent newNetworkEvent);

// Process a data packet received from the server
void processPacketFromServer (ENetPacket *newDataPacket, size_t dataSize);

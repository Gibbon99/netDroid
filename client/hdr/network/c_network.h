#pragma once

#include "enet/enet.h"
#include "com_globals.h"
#include <SDL_mutex.h>
#include "classes/com_threadsEngine.h"
#include "com_netEvents.h"
#include "com_texture.h"
#include "../../hdr/main.h"
#include "../../hdr/system/c_clientPeerInfo.h"

extern int networkFetchDelayMS;

// Called at startup to register the network processing thread and function
bool startClientEventNetwork();

// Add a newly received event from the network to the processing queue
void addClientNetworkEvent(ENetEvent newNetworkEvent);

// Process a data packet received from the server
void processPacketFromServer (ENetPacket *newDataPacket, size_t dataSize);

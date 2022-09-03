#pragma once

#include <enet/enet.h>
#include <cstdio>
#include "com_globals.h"
#include "com_util.h"

//------------------------------------------------------------------------------------------------------------------
//
// Network event - detached thread
//
//------------------------------------------------------------------------------------------------------------------
class droidEventNetwork
{
public:
    explicit droidEventNetwork(ENetEvent event)
    {
        networkEvent = event;

		printf("Added networkEvent - type [ %s ]\n", getEventType(networkEvent.type).c_str());
    }

	ENetEvent networkEvent{};
};

// Send a data packet to the nominated peer
bool com_sendDataToPeer (ENetPeer *whichPeer, dataPacket &newDataPacket);

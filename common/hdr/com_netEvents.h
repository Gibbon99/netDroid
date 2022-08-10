#pragma once

#include <enet/enet.h>
#include <cstdio>
#include "com_globals.h"

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

		printf("Added networkEvent - type [ %i ]\n", networkEvent.type);
    }

	ENetEvent networkEvent{};
};

// Send a data packet to the nominated peer
void com_sendDataToPeer (ENetPeer *whichPeer, const dataPacket& newDataPacket);

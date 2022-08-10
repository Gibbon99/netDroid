#pragma once

#include <vector>
#include <string>
#include "enet/enet.h"
#include "com_globals.h"

extern std::vector<peerInformation>    peers;

// Add a newly connected client to the peer array
//
// Return the new peer ID - index into the array
int addNewPeer(ENetPeer *newPeer);

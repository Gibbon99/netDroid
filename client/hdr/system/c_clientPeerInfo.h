#pragma once

#include <string>
#include "com_globals.h"

extern peerInformation clientPeer;

// Update the clientPeer information with the ID from the server
void updateClientPeerID(const dataPacket& newDataPacket);

// Set a new client friendly name
void updateClientPeerUsername(const std::string &newUsername);
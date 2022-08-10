#include "../../hdr/system/c_clientPeerInfo.h"
#include "com_globals.h"

peerInformation     clientPeer{};

//----------------------------------------------------------------------------------------------------------------------
//
// Update the clientPeer information with the ID from the server
void updateClientPeerID(int newID)
//----------------------------------------------------------------------------------------------------------------------
{
	clientPeer.ID = newID;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set a new client friendly name
void updateClientPeerUsername(const std::string &newUsername)
//----------------------------------------------------------------------------------------------------------------------
{
	clientPeer.userName = newUsername;
	if (clientPeer.ID != -1)
	{
		// TODO: Send packet to server with new name - pass in ID
	}
}
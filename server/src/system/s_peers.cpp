#include <cstring>
#include "../../hdr/system/s_peers.h"
#include "com_util.h"

std::vector<peerInformation> peers;

//----------------------------------------------------------------------------------------------------------------------
//
// Add a newly connected client to the peer array
//
// Return the new peer ID - index into the array
int addNewPeer (ENetPeer *newPeer)
//----------------------------------------------------------------------------------------------------------------------
{
	peerInformation newClientPeer;

	memcpy(&newClientPeer.peerInfo, newPeer, sizeof(newClientPeer.peerInfo));

	newClientPeer.userName = sys_getString ("netDroid-%s", getHostnameFromAddress (newClientPeer.peerInfo.address).c_str ());

	peers.push_back (newClientPeer);

	return static_cast<int>(peers.size () - 1);
}
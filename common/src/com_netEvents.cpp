#include <iostream>
#include "com_netEvents.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Send a data packet to the nominated peer
// bool com_sendDataToPeer (ENetPeer *whichPeer, const dataPacket &newDataPacket)
bool com_sendDataToPeer (ENetPeer *whichPeer, dataPacket &newDataPacket)
//----------------------------------------------------------------------------------------------------------------------
{
	pods::ResizableOutputBuffer             out;
	pods::MsgPackSerializer<decltype (out)> serializer (out);

	if (serializer.save (newDataPacket) != pods::Error::NoError)
	{
		std::cout << "Serialization error sending data to peer." << std::endl;
		return false;
	}

	ENetPacket *newPacket = enet_packet_create (out.data (), out.size (), ENET_PACKET_FLAG_RELIABLE);
	if (nullptr == newPacket)
	{
		std::cout << "Unable to create new data packet." << std::endl;
		enet_packet_destroy (newPacket);    // TODO Needed ??
		return false;
	}

	std::cout << "sendDataToPeer to [ " << getHostnameFromAddress (whichPeer->address) << ":" << whichPeer->address.port << " ] " << "  peer.state : " << getPeerState (whichPeer->state) << std::endl;

	if (enet_peer_send (whichPeer, 0, newPacket) < 0)
	{
		std::cout << "Unable to send data packet to peer." << std::endl;
		return false;
	}

	return true;
}
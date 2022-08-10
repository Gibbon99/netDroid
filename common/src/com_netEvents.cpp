#include <iostream>
#include "com_netEvents.h"
#include "com_globals.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Send a data packet to the nominated peer
void com_sendDataToPeer (ENetPeer *whichPeer, const dataPacket &newDataPacket)
//----------------------------------------------------------------------------------------------------------------------
{
	pods::ResizableOutputBuffer             out;
	pods::MsgPackSerializer<decltype (out)> serializer (out);

	if (serializer.save (newDataPacket) != pods::Error::NoError)
	{
		std::cout << "Serialization error sending data to peer." << std::endl;
	}

	ENetPacket *newPacket = enet_packet_create (out.data(), out.size (), ENET_PACKET_FLAG_RELIABLE);
	if (nullptr == newPacket)
	{
		printf ("Unable to create new data packet.\n");
		return;
	}

	if (enet_peer_send (whichPeer, 0, newPacket) < 0)
	{
		printf ("Unable to send data packet to peer.\n");
		return;
	}

//	enet_packet_destroy (newPacket);
}
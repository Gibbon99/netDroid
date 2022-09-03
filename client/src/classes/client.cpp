#include "../../hdr/classes/client.h"
#include "../../hdr/main.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Start the client networking
bool droidClient::initClient ()
//----------------------------------------------------------------------------------------------------------------------
{
	if (enet_initialize () != 0)
		return false;

	client = enet_host_create (nullptr,  // create a client host
	                           1,        // only allow 1 outgoing connection
	                           2,        // allow up 2 channels to be used, 0 and 1
	                           0,        // assume any amount of incoming bandwidth
	                           0         // assume any amount of outgoing bandwidth
	);
	if (client == nullptr)
	{
		enet_deinitialize ();
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Destroy client networking
void droidClient::destroyClient ()
//----------------------------------------------------------------------------------------------------------------------
{
	enet_host_destroy (client);
	enet_deinitialize ();
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return ENetHost pointer
ENetHost *droidClient::getHostPointer ()
//----------------------------------------------------------------------------------------------------------------------
{
	return client;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return a pointer to the peer informtion
ENetPeer *droidClient::getPeerPointer ()
//----------------------------------------------------------------------------------------------------------------------
{
	return peer;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Connect to a server
bool droidClient::connectToServer (const std::string &newServerHostname, int newServerPort)
//----------------------------------------------------------------------------------------------------------------------
{
	enet_address_set_host (&serverAddress, newServerHostname.c_str ());
	serverAddress.port = newServerPort;
	//
	// Initiate the connection, allocating the two channels 0 and 1.
	peer = enet_host_connect (client, &serverAddress, 2, 0);
	if (peer == nullptr)
		return false;

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Called from ENET_EVENT_CONNECT - which is when the connection is really complete
void droidClient::setConnectionComplete(bool newConnectionState)
//----------------------------------------------------------------------------------------------------------------------
{
	connectionComplete = newConnectionState;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Get if the connection is really complete or not
bool droidClient::getConnectionComplete()
//----------------------------------------------------------------------------------------------------------------------
{
	return connectionComplete;
}
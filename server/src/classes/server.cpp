#include <iostream>
#include "../../hdr/classes/server.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Start the server - pass in listen port and maximum number of clients
bool droidServer::initServer (const std::string &ip4Address, int maxNumClients, int listenPort)
//----------------------------------------------------------------------------------------------------------------------
{
	if (enet_initialize () != 0)
		return false;

	enet_address_set_host (&address, ip4Address.c_str ());
	//
	// Bind the server to listenPort
	address.port = listenPort;

	server = enet_host_create (&address,       // The address to bind the server host to
	                           maxNumClients,   // allow up to 32 clients and/or outgoing connections
	                           2,               // allow up to 2 channels to be used, 0 and 1
	                           0,               // assume any amount of incoming bandwidth
	                           0                // assume any amount of outgoing bandwidth
	);

	if (server == nullptr)
	{
		enet_deinitialize ();
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Destroy server
void droidServer::destroyServer ()
//----------------------------------------------------------------------------------------------------------------------
{
	enet_host_destroy (server);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return ENetHost pointer
ENetHost *droidServer::getHostPointer ()
//----------------------------------------------------------------------------------------------------------------------
{
	return server;
}
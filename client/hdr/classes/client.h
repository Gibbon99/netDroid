#pragma once

#include <string>
#include "enet/enet.h"

class droidClient
{
public:

	bool initClient ();

	void destroyClient ();

	ENetHost *getHostPointer ();

	bool connectToServer (const std::string &newServerHostname, int newServerPort);

private:
	ENetHost    *client{};
	ENetAddress serverAddress{};
	ENetPeer    *peer{};
	ENetEvent   event{};
	std::string serverHostname{};
};

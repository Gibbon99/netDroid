#pragma once

#include <string>
#include "enet/enet.h"

class droidClient
{
public:

	bool initClient ();

	void destroyClient ();

	ENetHost *getHostPointer ();

	ENetPeer *getPeerPointer ();

	bool connectToServer (const std::string &newServerHostname, int newServerPort);

	void setConnectionComplete(bool newConnectionState);

	bool getConnectionComplete();
private:
	ENetHost    *client{};
	ENetAddress serverAddress{};
	ENetPeer    *peer{};
	ENetEvent   event{};
	std::string serverHostname{};
	bool        connectionComplete{false};
};

#pragma once

#include <enet/enet.h>
#include <string>

#ifdef __WIN32__
#include "SDL.h"    // Change to windows signal
#else

#include <csignal>

#endif

class droidServer {
public:
    bool initServer(const std::string &ip4Address, int maxNumClients, int listenPort);

    void destroyServer();

    ENetHost *getHostPointer();

private:
    ENetAddress address;
    ENetHost *server;
};

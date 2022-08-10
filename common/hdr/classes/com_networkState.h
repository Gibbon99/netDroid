#pragma once

#include <SDL_mutex.h>

enum networkStates {
    NETWORK_STATE_PRE_INIT,
    NETWORK_STATE_DO_INIT,
    NETWORK_STATE_INIT_ERROR,
    NETWORK_STATE_INIT_DONE,
    NETWORK_STATE_CONNECTED,
    NETWORK_STATE_ATTEMPTING_CONNECT,
    NETWORK_STATE_CONNECT_FAILED,
    NETWORK_STATE_CONNECT_FAILED_ALERT,
    NETWORK_STATE_CONNECT_SUCCESS,
    NETWORK_STATE_KICKED,
    NETWORK_STATE_TIMED_OUT,
    NETWORK_STATE_TRY_SERVER_CONNECT
};

class networkState {

public:

    networkState();
    ~networkState();

    void setNewState(int newState);

    void setNextState(int newState);

    int getCurrentState();

    bool isNetworkStarted();

    void setNetworkStarted(int newState);

private:
    int currentState = NETWORK_STATE_PRE_INIT;
    int nextState = NETWORK_STATE_DO_INIT;
    int previousState{};
    bool networkStarted = false;

    SDL_mutex  *mutex = nullptr;

    int peerID{};   // TODO
};

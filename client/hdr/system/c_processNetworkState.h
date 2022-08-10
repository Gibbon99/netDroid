#pragma once

#include "../main.h"
#include "classes/com_threadsEngine.h"

extern droidThreadsEngine monitorNetworkStateThread;

// Start a thread to manage the client network textureState
bool startNetworkStateThread();
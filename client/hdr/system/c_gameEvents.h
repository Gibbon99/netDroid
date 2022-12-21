#pragma once

#include <SDL2/SDL_mutex.h>
#include "com_threadsEngine.h"

extern droidThreadsEngine  mainLoopMutex;

// Setup the mutex for the game loop queue access
bool c_createGameLoopMutex();

// Handle the events that need to be run in the main loop
// eg: Changes to physics world, upload GL textures
void c_processGameEventQueue();
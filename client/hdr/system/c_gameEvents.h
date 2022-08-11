#pragma once

#include <SDL_mutex.h>

extern SDL_mutex *mainLoopMutex;

// Setup the mutex for the game loop queue access
bool c_createGameLoopMutex();

// Handle the events that need to be run in the main loop
// eg: Changes to physics world, upload GL textures
int c_processGameEventQueue();
#pragma once

#include <vector>
#include <string>
#include <queue>
#include <SDL_timer.h>
#include <SDL_mutex.h>
#include <SDL_thread.h>
#include "com_util.h"

//
// This class holds all the registered thread and queue mutexes.
// Threads are registered on startup and call back into main game to process the actual event type
//

class droidThreadsEngine {

public:

    std::string getErrorString();

    std::string sys_getString(std::string formatIn, ...);

    bool canThreadRun(const std::string &threadName);

    bool setThreadReadyState(bool newState, const std::string &threadName);

    bool isThreadReady(const std::string &threadName);

    bool setThreadRunState(int newState, const std::string &threadName);

    void stopThreads();

    bool registerThread(SDL_ThreadFunction threadFunction, const std::string &threadName);

    bool registerMutex(const std::string &mutexName);

    SDL_mutex *findMutex(const std::string &mutexName);

    void destroyMutexes();

private:

    typedef struct droidRegisteredThreads
    {
        SDL_Thread *thread = nullptr;
        bool        run     = false;
        bool        ready   = false;
        std::string name{};
    } droidRegisteredThreads;

    typedef struct droidRegisteredMutexes
    {
        SDL_mutex  *mutex = nullptr;
        std::string name{};
    } droidRegisteredMutexes;

    std::vector<droidRegisteredThreads> registeredThreads {};
    std::vector<droidRegisteredMutexes> registeredMutexes {};

    bool runThreads = true;     // Master flag to control textureState of detached threads
    std::string lastError;      // Store text for description of last error message
};
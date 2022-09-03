#include "classes/com_threadsEngine.h"

//
// registerThreads.empty profiles at 9%;
// registerThreads.size() == 0 profiles at < 1%
//

//----------------------------------------------------------------------------------------------------------------------
//
// Return the last error message string
std::string droidThreadsEngine::getErrorString ()
//----------------------------------------------------------------------------------------------------------------------
{
	return lastError;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Pass in string and parameters to format and return a string
std::string droidThreadsEngine::sys_getString (std::string formatIn, ...)
//----------------------------------------------------------------------------------------------------------------------
{
	std::string s{};

	va_list args, args2;
	va_start(args, formatIn);
	va_copy(args2, args);
	//
	// Get string length
	s.resize (vsnprintf (nullptr, 0, formatIn.c_str (), args2) + 1);
	va_end(args2);
	//
	// Actually write the string and args
	vsprintf (s.data (), formatIn.c_str (), args);
	va_end(args);
	s.pop_back ();
	//
	// Return the full string
	return s;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the state 'run' for this thread passed by name
// Return error message in 'm_lastError' string if required
bool droidThreadsEngine::canThreadRun (const std::string &threadName)
//----------------------------------------------------------------------------------------------------------------------
{
	if (registeredThreads.size () == 0)
	{
		lastError = sys_getString ("No registered threads - [ %s ]", threadName.c_str ());
		return false;
	}

	for (auto &threadItr: registeredThreads)
	{
		if (threadName == threadItr.name)
		{
			lastError = "No error";
			return threadItr.run;
		}
	}
	lastError = sys_getString ("Thread not found - [ %s ]", threadName.c_str ());
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Control thread running and processing based on thread name
bool droidThreadsEngine::setThreadRunState (int newState, const std::string &threadName)
//----------------------------------------------------------------------------------------------------------------------
{
	for (auto &threadItr: registeredThreads)
	{
		if (threadName == threadItr.name)
		{
			lastError = "";
			threadItr.run = newState;
			return true;
		}
	}
	lastError = sys_getString ("Attempting to set run state for unknown thread [ %s ].", __func__, threadName.c_str ());
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set thread ready
bool droidThreadsEngine::setThreadReadyState (bool newState, const std::string &threadName)
//----------------------------------------------------------------------------------------------------------------------
{
	for (auto &threadItr: registeredThreads)
	{
		if (threadName == threadItr.name)
		{
			lastError = "";
			threadItr.ready = newState;
			return true;
		}
	}

	lastError = sys_getString ("Attempting to set ready textureState for unknown thread [ %s ].", threadName.c_str ());
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
//
// See if the thread is ready or not
bool droidThreadsEngine::isThreadReady (const std::string &threadName)
//----------------------------------------------------------------------------------------------------------------------
{
	for (const auto &threadItr: registeredThreads)
	{
		if (threadName == threadItr.name)
		{
			lastError = "No error";
			return threadItr.ready;
		}
	}
	lastError = sys_getString ("Thread [ %s ] not found.", threadName.c_str ());
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Stop threads
void droidThreadsEngine::stopThreads ()
//----------------------------------------------------------------------------------------------------------------------
{
	runThreads = false;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Keep a list of threads that have been created.
bool droidThreadsEngine::registerThread (SDL_ThreadFunction threadFunction, const std::string &threadName)
//----------------------------------------------------------------------------------------------------------------------
{
	droidRegisteredThreads newThread;

	newThread.name  = threadName;
	newThread.run   = true;
	newThread.ready = true;
	//
	// Create vector element first
	// Thread was being created too fast and running before the vector element was created
	// resulting in no elements being available when size() was tested
	registeredThreads.push_back (newThread);
	SDL_Delay (100);
	registeredThreads[registeredThreads.size () - 1].thread = SDL_CreateThread (*threadFunction, threadName.c_str (), nullptr);
	if (nullptr == registeredThreads[registeredThreads.size () - 1].thread)
	{
		lastError = sys_getString ("Unable to create thread [ %s ] - [ %s ]", threadName.c_str (), SDL_GetError ());
		return false;
	}

	setThreadReadyState (false, threadName);
	SDL_DetachThread (registeredThreads[registeredThreads.size () - 1].thread);
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Keep a list of mutexes that have been created, and remove at shutdown
bool droidThreadsEngine::registerMutex (const std::string &mutexName)
//----------------------------------------------------------------------------------------------------------------------
{
	droidRegisteredMutexes newMutex;

	newMutex.mutex = SDL_CreateMutex ();
	newMutex.name  = mutexName;
	if (nullptr == newMutex.mutex)
	{
		lastError = sys_getString ("Unable to create mutex [ %s ] - [ %s ]", newMutex.name.c_str (), SDL_GetError ());
		return false;
	}
	registeredMutexes.push_back (newMutex);

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Find a mutex based on its name
SDL_mutex *droidThreadsEngine::findMutex (const std::string &mutexName)
//----------------------------------------------------------------------------------------------------------------------
{
	for (const auto &mutexItr: registeredMutexes)
	{
		if (mutexItr.name == mutexName)
		{
			lastError = "";
			return mutexItr.mutex;
		}
	}

	lastError = sys_getString ("Unable to locate mutex [ %s ]", mutexName.c_str ());
	return nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Destroy all the mutexes
void droidThreadsEngine::destroyMutexes ()
//----------------------------------------------------------------------------------------------------------------------
{
	for (auto &mutexItr: registeredMutexes)
	{
		SDL_DestroyMutex (mutexItr.mutex);
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Lock the mutex - return false on error
bool droidThreadsEngine::lockMutex(SDL_mutex *mutexName)
//----------------------------------------------------------------------------------------------------------------------
{
	if (SDL_LockMutex (mutexName) < 0)
	{
		lastError = sys_getString("Unable to lock mutex [ %s ]", SDL_GetError());
		return false;
	}
	else
	{
		lastError = "No error";
		return true;
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Unlock the mutex - return false on error
bool droidThreadsEngine::unLockMutex(SDL_mutex *mutexName)
//----------------------------------------------------------------------------------------------------------------------
{
	if (SDL_UnlockMutex (mutexName) < 0)
	{
		lastError = sys_getString("Unable to unlock mutex [ %s ]", SDL_GetError());
		return false;
	}
	else
	{
		lastError = "No error";
		return true;
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Lock the mutex - return false on error
bool droidThreadsEngine::lockMutex(const std::string &mutexName)
//----------------------------------------------------------------------------------------------------------------------
{
	for (const auto &mutexItr: registeredMutexes)
	{
		if (mutexItr.name == mutexName)
		{
			if (SDL_LockMutex (mutexItr.mutex) < 0)
			{
				lastError = sys_getString("Unable to lock mutex [ %s ] : [ %s ]", mutexName.c_str(), SDL_GetError());
				return false;
			}
			else
			{
				lastError = "No error";
				return true;
			}
		}
	}
	lastError = sys_getString("Unable to find mutex [ %s ]", mutexName.c_str());
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
//
// UnLock the mutex - return false on error
bool droidThreadsEngine::unLockMutex(const std::string &mutexName)
//----------------------------------------------------------------------------------------------------------------------
{
	for (const auto &mutexItr: registeredMutexes)
	{
		if (mutexItr.name == mutexName)
		{
			if (SDL_UnlockMutex(mutexItr.mutex) < 0)
			{
				lastError = sys_getString("Unable to unLock mutex [ %s ] : [ %s ]", mutexName.c_str(), SDL_GetError());
				return false;
			}
			else
			{
				lastError = "No error";
				return true;
			}
		}
	}
	lastError = sys_getString("Unable to find mutex [ %s ]", mutexName.c_str());
	return false;
}
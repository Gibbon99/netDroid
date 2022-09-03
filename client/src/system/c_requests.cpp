#include <utility>

#include "../../hdr/system/c_requests.h"
#include "../../hdr/main.h"

int                       maxRequestCount{5000};
int                       requestsFetchDelayMS{50};
std::vector<requestType_> requestQueue{};

//----------------------------------------------------------------------------------------------------------------------
//
// Process the request queue to see if we need to request again
int c_processRequestQueue ([[maybe_unused]]void *param)
//----------------------------------------------------------------------------------------------------------------------
{
	static SDL_mutex *requestsMutex{nullptr};       // Cache the mutex value
	auto             index{0};

	//
	// Cache getting the mutex value
	requestsMutex = clientThreads.findMutex (EVENT_CLIENT_REQUESTS_MUTEX_NAME);
	if (nullptr == requestsMutex)
	{
		clientMessage.message (MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_CONSOLE, sys_getString ("%s", clientThreads.getErrorString ().c_str ()));
		// TODO Quit with error
	}

	while (clientThreads.canThreadRun (EVENT_CLIENT_REQUESTS_THREAD_NAME))
	{
		if (clientThreads.isThreadReady (EVENT_CLIENT_REQUESTS_THREAD_NAME))
		{

			SDL_Delay (requestsFetchDelayMS);

			if (!requestQueue.empty ())   // Events in the queue to process
			{
				if (clientThreads.lockMutex (requestsMutex))   // Blocks if the mutex is locked by another thread
				{
					for (auto requestItr: requestQueue)
					{
						if (gameTime.getTicks () > static_cast<Uint32>(requestItr.requestTime + maxRequestCount))
						{
							//
							// Resend request to server TODO
//							c_sendRequestToServer (requestItr.requestString, requestItr.requestType);
							requestQueue.erase (requestQueue.begin () + index);
						}
						index++;
					}
					clientThreads.unLockMutex (requestsMutex);
				}
			}
		}
	}
	return 1;
}

//----------------------------------------------------------------------------------------------------------------------
//
// A Request has been fulfilled - so remove it from the queue
void c_removeRequest (std::string_view requestName)
//----------------------------------------------------------------------------------------------------------------------
{
	static SDL_mutex *requestsMutex{nullptr};       // Cache the mutex value

	//
	// Cache getting the mutex value
	requestsMutex = clientThreads.findMutex (EVENT_CLIENT_REQUESTS_MUTEX_NAME);
	if (nullptr == requestsMutex)
	{
		clientMessage.message (MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_CONSOLE, sys_getString ("%s", clientThreads.getErrorString ().c_str ()));
		// TODO Quit with error
	}

	if (clientThreads.lockMutex (requestsMutex))   // Blocks if the mutex is locked by another thread
	{
		auto index = 0;

		for (auto requestItr: requestQueue)
		{
			if (requestItr.requestString == requestName)
			{
				//
				// Remove the request from our queue - the request has been fulfilled.
				requestQueue.erase (requestQueue.begin () + index);
			}
			else
			{
				index++;
			}
		}
		clientThreads.unLockMutex (requestsMutex);
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Init the request queue - spin up a thread to monitor and reissue requests if the requestCount is too high
bool c_initRequestQueue ()
//----------------------------------------------------------------------------------------------------------------------
{

	if (!clientThreads.registerMutex (EVENT_CLIENT_REQUESTS_MUTEX_NAME))
	{
		clientMessage.message (MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, clientThreads.getErrorString ());
		return false;
	}
	else
		clientMessage.message (MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, sys_getString ("Mutex [ %s ] registered.", EVENT_CLIENT_REQUESTS_MUTEX_NAME));

	if (!clientThreads.registerThread (c_processRequestQueue, EVENT_CLIENT_REQUESTS_THREAD_NAME))
	{
		clientMessage.message (MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, clientThreads.getErrorString ());
		return false;
	}
	else
		clientMessage.message (MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, sys_getString ("Thread [ %s ] registered.", EVENT_CLIENT_REQUESTS_THREAD_NAME));

	if (!clientThreads.setThreadRunState (true, EVENT_CLIENT_REQUESTS_THREAD_NAME))
	{
		clientMessage.message (MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, clientThreads.getErrorString ());
		return false;
	}

	if (!clientThreads.setThreadReadyState (true, EVENT_CLIENT_REQUESTS_THREAD_NAME))
	{
		clientMessage.message (MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, clientThreads.getErrorString ());
		return false;
	}
	return true;

}

//----------------------------------------------------------------------------------------------------------------------
//
// Add a new request to the queue
void c_addRequestToQueue (std::string newRequestString, DATA_PACKET_TYPES newRequestType)
//----------------------------------------------------------------------------------------------------------------------
{
	requestType_ tempRequestType{};

	tempRequestType.requestString = std::move (newRequestString);
	tempRequestType.requestTime   = static_cast<int>(gameTime.getTicks ());
	tempRequestType.requestType   = newRequestType;
	tempRequestType.requestCount  = 1;

	if (clientThreads.lockMutex (EVENT_CLIENT_REQUESTS_MUTEX_NAME))   // Blocks if the mutex is locked by another thread
	{
		requestQueue.push_back (tempRequestType);
		clientThreads.unLockMutex (EVENT_CLIENT_REQUESTS_MUTEX_NAME);
	}
}

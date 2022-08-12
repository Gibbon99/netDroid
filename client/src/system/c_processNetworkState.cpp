#include "../../hdr/system/c_processNetworkState.h"
#include "../../hdr/system/c_clientPeerInfo.h"

int         serverPort = 1234;
std::string serverAddress{"127.0.0.1"};

#define EVENT_CLIENT_NETWORK_MONITOR_THREAD    "monitorNetworkStateThread"

droidThreadsEngine monitorNetworkStateThread{};

//----------------------------------------------------------------------------------------------------------------------
//
// Run this to check on the current network textureState of the client.  Take action depending on the textureState
int processNetworkState (void *param)
//----------------------------------------------------------------------------------------------------------------------
{
	while (monitorNetworkStateThread.canThreadRun (EVENT_CLIENT_NETWORK_MONITOR_THREAD))
	{
		if (monitorNetworkStateThread.isThreadReady (EVENT_CLIENT_NETWORK_MONITOR_THREAD))
		{
			SDL_Delay (THREAD_DELAY_MS);

			switch (clientNetworkState.getCurrentState ())
			{
				case networkStates::NETWORK_STATE_PRE_INIT:
					clientNetworkState.setNetworkStarted (false);
					clientNetworkState.setNewState (networkStates::NETWORK_STATE_DO_INIT);
					break;

				case networkStates::NETWORK_STATE_DO_INIT:
					if (!clientNetworkObject.initClient ())
					{
						clientNetworkState.setNewState (networkStates::NETWORK_STATE_INIT_ERROR);
						clientMessage.message(MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, "An error occurred starting network library.");
					}
					else
					{
						clientNetworkState.setNewState (networkStates::NETWORK_STATE_INIT_DONE);
						clientNetworkState.setNetworkStarted (true);
						clientMessage.message(MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, "Network library started.");
					}
					break;

				case networkStates::NETWORK_STATE_INIT_ERROR:
					break;

				case networkStates::NETWORK_STATE_TRY_SERVER_CONNECT:
					clientMessage.message(MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, "Attempt connection to server...");
					clientNetworkState.setNewState (networkStates::NETWORK_STATE_ATTEMPTING_CONNECT);
					if (clientNetworkState.isNetworkStarted ())
					{
						if (!clientNetworkObject.connectToServer (serverAddress, serverPort))
						{
							clientMessage.message(MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, "No available peers for initiating an ENet connection.");
							clientNetworkState.setNewState (networkStates::NETWORK_STATE_CONNECT_FAILED_ALERT);
						}
						else
						{
							clientMessage.message(MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, "Connection request sent to server.");
						}
					}
					else
					{
						clientMessage.message(MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, "Network stack has not been started.");
						clientNetworkState.setNewState (networkStates::NETWORK_STATE_PRE_INIT);
					}
					break;

				case networkStates::NETWORK_STATE_ATTEMPTING_CONNECT:
					static int attemptCounter = 0;
					SDL_Delay (1000);
					clientMessage.message(MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, sys_getString("[ %i ] Attempting to connect to server.", attemptCounter));
					attemptCounter++;
					if (attemptCounter > 5)
					{
						attemptCounter = 0;
						clientNetworkState.setNewState (networkStates::NETWORK_STATE_CONNECT_FAILED_ALERT);
					}
					break;

				case networkStates::NETWORK_STATE_CONNECT_FAILED_ALERT:
					clientMessage.message(MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, "Connection failed.");
					clientNetworkState.setNewState (networkStates::NETWORK_STATE_CONNECT_FAILED);
					break;

				case networkStates::NETWORK_STATE_CONNECT_FAILED:
					// Send an event to the game thread starting the process again - limit number of times
					break;

				case networkStates::NETWORK_STATE_INIT_DONE:
					break;

				case networkStates::NETWORK_STATE_CONNECT_SUCCESS:
					clientMessage.message(MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, "Connected to server.");
					clientNetworkState.setNewState (networkStates::NETWORK_STATE_CONNECTED);
					break;

				case networkStates::NETWORK_STATE_CONNECTED:
					break;

				case networkStates::NETWORK_STATE_KICKED:
					break;

				case networkStates::NETWORK_STATE_TIMED_OUT:
					break;
			}
		}
	}
	clientMessage.message(MESSAGE_TARGET_STD_OUT, "Network check thread has stopped.");
	return 1;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Start a thread to manage the client network textureState
bool startNetworkStateThread ()
//----------------------------------------------------------------------------------------------------------------------
{
	clientNetworkState.setNewState (networkStates::NETWORK_STATE_PRE_INIT);

	if (!monitorNetworkStateThread.registerThread (reinterpret_cast<SDL_ThreadFunction>(processNetworkState), EVENT_CLIENT_NETWORK_MONITOR_THREAD))
	{
		clientMessage.message(MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, monitorNetworkStateThread.getErrorString ());
		return false;
	}
	else
		clientMessage.message(MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, sys_getString("Thread [ %s ] registered.", EVENT_CLIENT_NETWORK_MONITOR_THREAD));

	if (!monitorNetworkStateThread.setThreadReadyState (true, EVENT_CLIENT_NETWORK_MONITOR_THREAD))
	{
		clientMessage.message(MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, monitorNetworkStateThread.getErrorString());
		return false;
	}

	if (!monitorNetworkStateThread.setThreadRunState (true, EVENT_CLIENT_NETWORK_MONITOR_THREAD))
	{
		clientMessage.message(MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, monitorNetworkStateThread.getErrorString());
		return false;
	}

	return true;
}
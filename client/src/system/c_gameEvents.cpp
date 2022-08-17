#include "../../hdr/system/c_gameEvents.h"
#include "../../hdr/main.h"
#include "../../hdr/system/c_events.h"

//Add game event queue
std::queue<myEventData_> gameEventQueue;

//----------------------------------------------------------------------------------------------------------------------
//
// Setup the mutex for the game loop queue access
bool c_createGameLoopMutex()
//----------------------------------------------------------------------------------------------------------------------
{
	if (!clientThreads.registerMutex (MAIN_LOOP_MUTEX))
	{
		clientMessage.message(MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT, sys_getString("Unable to create mainLoopMutex : [ %s ]", clientThreads.getErrorString().c_str()));
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Handle the events that need to be run in the main loop
// eg: Changes to physics world, upload GL textures
void c_processGameEventQueue()
//----------------------------------------------------------------------------------------------------------------------
{
	myEventData_ tempEventData;

	if ( !gameEventQueue.empty() )
	{
		if ( clientThreads.lockMutex ( MAIN_LOOP_MUTEX ))
		{
			tempEventData = gameEventQueue.front();
			gameEventQueue.pop ();
			clientThreads.unLockMutex( MAIN_LOOP_MUTEX );
		}

		switch ( tempEventData.eventAction )
		{
			case EventAction::ACTION_UPLOAD_TEXTURE:
				if (!clientTextures[tempEventData.eventString].convertToTexture())
				{
				clientMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_CONSOLE, sys_getString("%s", clientTextures[tempEventData.eventString].getLastError().c_str()));
				return;
				}
				break;

			case EventAction::ACTION_CONSOLE_ADD:
				clientConsole.add(0, tempEventData.data1, tempEventData.data2, tempEventData.data3, 1, tempEventData.eventString);
				break;

			default:
				break;
		}
	}
}

#include "../../hdr/system/c_gameEvents.h"
#include "../../hdr/main.h"
#include "../../hdr/system/c_events.h"

//Add game event queue
std::queue<myEventData_> gameEventQueue;

SDL_mutex *mainLoopMutex;

//----------------------------------------------------------------------------------------------------------------------
//
// Setup the mutex for the game loop queue access
bool c_createGameLoopMutex()
//----------------------------------------------------------------------------------------------------------------------
{
	mainLoopMutex = SDL_CreateMutex();
	if (nullptr == mainLoopMutex)
	{
		clientMessage.message(MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT, sys_getString("Unable to create mainLoopMutex : [ %s ]", SDL_GetError()));
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
		if ( SDL_LockMutex ( mainLoopMutex ) == 0)
		{
			tempEventData = gameEventQueue.front();
			gameEventQueue.pop ();
			SDL_UnlockMutex (mainLoopMutex);
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

			default:
				break;
		}
	}
}

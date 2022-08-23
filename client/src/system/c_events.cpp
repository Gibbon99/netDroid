#include "../../hdr/system/c_events.h"
#include "../../hdr/system/c_audio.h"

//------------------------------------------------------------------------
//
// Create a custom event and add it to the relevant queue
void c_addEventToQueue (EventType type, EventAction action, int data1, int data2, int data3, const glm::vec2 vec2_1, const glm::vec2 vec2_2, std::string textString )
//------------------------------------------------------------------------
{
	myEventData_ eventData;

	eventData.eventType   = type;
	eventData.eventAction = action;
	eventData.data1       = data1;
	eventData.data2       = data2;
	eventData.data3       = data3;
	eventData.vec2_1      = vec2_1;
	eventData.vec2_2      = vec2_2;
	eventData.eventString = std::move (textString);

	switch (type)
	{
		case EventType::EVENT_GAME_LOOP:

			if (clientThreads.lockMutex ( MAIN_LOOP_MUTEX ))
			{
				gameEventQueue.push (eventData);
				clientThreads.unLockMutex ( MAIN_LOOP_MUTEX );
			}
			break;

		case EventType::EVENT_CONSOLE:
			if (clientThreads.lockMutex ( MAIN_LOOP_MUTEX ))
			{
				gameEventQueue.push (eventData);
				clientThreads.unLockMutex ( MAIN_LOOP_MUTEX );
			}
			break;
	}

}


//----------------------------------------------------------------------------------------------------------------------
//
// Handle SDL events from main loop
void c_handleEvents(SDL_Event event)
//----------------------------------------------------------------------------------------------------------------------
{
//	ImGui_ImplSDL2_ProcessEvent (&event);

	//
	// Check for OS Windowing events
	if (event.type == SDL_WINDOWEVENT)
	{
		switch (event.window.event)
		{
			case SDL_WINDOWEVENT_FOCUS_LOST:
				break;

			case SDL_WINDOWEVENT_FOCUS_GAINED:
				break;

			case SDL_WINDOWEVENT_SHOWN:
				break;

			case SDL_WINDOWEVENT_CLOSE:
				quitLoop = true;
				break;
		}
	}

	if (event.type == SDL_KEYDOWN)
	{

		printf("Play a loaded sample\n");
		c_playSample("scrollBeeps", false);
	}
}

//------------------------------------------------------------------------------------------------------------------
//
// Get any network events and put onto processing queue
void c_getNetworkEvents ()
//------------------------------------------------------------------------------------------------------------------
{
	ENetEvent event;

	while (enet_host_service (clientNetworkObject.getHostPointer (), &event, 0) > 0)
	{
		c_addNetworkEvent (event);
	}
}
#include "../../hdr/system/c_events.h"
#include "enet/enet.h"
#include "../../hdr/main.h"
#include "../../hdr/system/c_gameEvents.h"

//------------------------------------------------------------------------
//
// Create a custom event to be sent
void evt_sendEvent ( uint type, int action, int data1, int data2, int data3, const glm::vec2 vec2_1, const glm::vec2 vec2_2, std::string textString )
//------------------------------------------------------------------------
{
	_myEventData eventData;

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
		case GAME_LOOP_EVENT:
			if (SDL_LockMutex (mainLoopMutex) == 0)
			{
				gameEventQueue.push (eventData);
				SDL_UnlockMutex (mainLoopMutex);
			}
			break;
	}

}


//----------------------------------------------------------------------------------------------------------------------
//
// Handle SDL events from main loop
void handleEvents(SDL_Event event)
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
}

//------------------------------------------------------------------------------------------------------------------
//
// Get any network events and put onto processing queue
void getClientNetworkEvents ()
//------------------------------------------------------------------------------------------------------------------
{
	ENetEvent event;
	int       returnCode{};

	returnCode = enet_host_service (clientNetworkObject.getHostPointer (), &event, 0);
	if (returnCode == 0)    // No events
		return;

	if (returnCode > 0)
	{
		addClientNetworkEvent (event);
		return;
	}

	if (returnCode < 0)
	{
		// TODO Report an error
		return;
	}
}
#include "../../hdr/system/c_processClientFrame.h"
#include "../../hdr/main.h"

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
// Called each frame to process events
void processClientFrame ()
//------------------------------------------------------------------------------------------------------------------
{
	SDL_Event evt{};

    getClientNetworkEvents ();

	while (SDL_PollEvent(&evt) > 0)
	{
		handleEvents(evt);
	}
}


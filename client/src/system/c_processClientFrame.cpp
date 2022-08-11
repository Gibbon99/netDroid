#include "../../hdr/system/c_processClientFrame.h"
#include "../../hdr/main.h"
#include "../../hdr/system/c_gameEvents.h"
#include "../../hdr/system/c_events.h"



//------------------------------------------------------------------------------------------------------------------
//
// Called each frame to process events
void processClientFrame ()
//------------------------------------------------------------------------------------------------------------------
{
	SDL_Event evt{};

    getClientNetworkEvents ();
	c_processGameEventQueue();

	while (SDL_PollEvent(&evt) > 0)
	{
		handleEvents(evt);
	}
}


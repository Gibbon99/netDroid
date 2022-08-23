#include "../../hdr/system/c_processClientFrame.h"
#include "../../hdr/main.h"
#include "../../hdr/system/c_events.h"

//------------------------------------------------------------------------------------------------------------------
//
// Called each frame to process events
void processClientFrame ()
//------------------------------------------------------------------------------------------------------------------
{
	SDL_Event evt{};

	c_getNetworkEvents ();
	c_processGameEventQueue();

	while (SDL_PollEvent(&evt) > 0)
	{
		c_handleEvents (evt);
	}
}


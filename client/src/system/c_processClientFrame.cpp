#include "../../hdr/system/c_processClientFrame.h"
#include "../../hdr/main.h"
#include "../../hdr/system/c_events.h"
#include "../../hdr/system/c_util.h"

MODE_TYPES currentMode{MODE_TYPES::MODE_REQUEST_WAIT};

//------------------------------------------------------------------------------------------------------------------
//
// Called each frame to process events
void processClientFrame ()
//------------------------------------------------------------------------------------------------------------------
{
	SDL_Event evt{};
	static bool sentInitRequest{false};

	c_getNetworkEvents ();
	c_processGameEventQueue();

	switch (currentMode)
	{
		case MODE_TYPES::MODE_REQUEST_WAIT:
			break;

		case MODE_TYPES::MODE_REQUEST_INIT_SCRIPT:

			if (clientPeer.peerInfo.address.host != 0)
			{
				if (!sentInitRequest)
				{
					c_sendRequestToServer ("initScript", DATA_PACKET_TYPES::PACKET_REQUEST_INIT_SCRIPT);
					c_changeMode (MODE_TYPES::MODE_WAIT_FOR_INIT_SCRIPT);
					sentInitRequest = true;
				}
			}
			break;

		case MODE_TYPES::MODE_WAIT_FOR_INIT_SCRIPT:
			break;

		case MODE_TYPES::MODE_LOAD_RESOURCES:
			break;

		case MODE_TYPES::MODE_SPLASH_SCREEN:
			break;

		default:
			std::cout << "Unknown current mode." << std::endl;
			break;
	}
	while (SDL_PollEvent(&evt) > 0)
	{
		c_handleEvents (evt);
	}
}


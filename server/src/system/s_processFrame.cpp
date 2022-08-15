#include "../../hdr/system/s_processFrame.h"
#include "enet/enet.h"

//------------------------------------------------------------------------------------------------------------------
//
// Get any network events and put onto processing queue
void getNetworkEvents ()
//------------------------------------------------------------------------------------------------------------------
{
	ENetEvent event{};

	while (enet_host_service (serverObject.getHostPointer (), &event, 0) > 0)
	{
		addNetworkEvent (event);
	}
}

//------------------------------------------------------------------------------------------------------------------
//
// Called each frame to process events
void processFrame ()
//------------------------------------------------------------------------------------------------------------------
{
	getNetworkEvents ();
}


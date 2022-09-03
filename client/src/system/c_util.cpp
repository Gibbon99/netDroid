#include "../../hdr/system/c_util.h"

//------------------------------------------------------------------------------------------------------------------
//
// Return string for current mode
std::string c_getModeString(MODE_TYPES whichMode)
//------------------------------------------------------------------------------------------------------------------
{
	switch (whichMode)
	{
		case MODE_TYPES::MODE_REQUEST_WAIT:
			return "MODE_REQUEST_WAIT";
			break;

		case MODE_TYPES::MODE_REQUEST_INIT_SCRIPT:
			return "MODE_REQUEST_INIT_SCRIPT";
			break;

		case MODE_TYPES::MODE_WAIT_FOR_INIT_SCRIPT:
			return "MODE_WAIT_FOR_INIT_SCRIPT";
			break;

		case MODE_TYPES::MODE_LOAD_RESOURCES:
			return "MODE_LOAD_RESOURCES";
			break;

		case MODE_TYPES::MODE_SPLASH_SCREEN:
			return "MODE_SPLASH_SCREEN";
			break;
	}
}

//------------------------------------------------------------------------------------------------------------------
//
// Change the current game mode
void c_changeMode(MODE_TYPES newMode)
//------------------------------------------------------------------------------------------------------------------
{
	currentMode = newMode;

	std::cout << "Changing mode to [ " << c_getModeString(newMode) << " ]" << std::endl;
}
#include "../../hdr/system/c_console.h"
#include "../../hdr/main.h"

#define CONSOLE_MUTEX_NAME "consoleMutex"

//----------------------------------------------------------------------------------------------------------------------
//
// Start the console processing queue and thread
bool con_initConsole()
//----------------------------------------------------------------------------------------------------------------------
{
//	console.setScreenSize (hiresVirtualWidth, hiresVirtualHeight);
//	console.setNumVarColumns (consoleNumColumns);
	//
	// Start the console and processing thread
	if (!clientThreads.registerMutex (CONSOLE_MUTEX_NAME))
	{
		clientMessage.message(MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT, sys_getString("Unable to create console mutex : [ %s ]", clientThreads.getErrorString().c_str()));
		return false;
	}

	clientMessage.message(MESSAGE_TARGET_CONSOLE |  MESSAGE_TARGET_LOGFILE, "Console started.");

//	clientConsole.addCommand ("quit", "Quit the game.", sys_shutdown);

	//
	// Variables accessible from the console
	//
//	clientConsole.addVariable ("quitLoop", VAR_TYPE_BOOL, &quitLoop);

return true;
}

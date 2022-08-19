#include "../../hdr/system/c_console.h"
#include "../../hdr/main.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Render the lines in the console
void c_renderConsole ()
//----------------------------------------------------------------------------------------------------------------------
{
	auto lineCounter      = 0;
	int  numLinesToRender = clientWindow.getHeight () / clientTestFont.getHeight ();

	auto fromMessages = clientMessage.getStrings ();
	if (!fromMessages.empty ())
	{
		//
		// Transfer all the messages from message cache into the console queue
		for (const auto &messageItr: fromMessages)
		{
			clientConsole.add (messageItr);
		}
		clientMessage.clearStrings ();
	}

	clientConsole.prepare (clientConsole.getDefaultPosX (), clientTestFont.getHeight () * 2);

	auto nextRenderLine = clientConsole.getLine ();
	auto lineYPos{clientTestFont.getHeight ()};
	while (nextRenderLine.size () != 0)
	{
		clientTestFont.addText (glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, glm::vec2{clientConsole.getDefaultPosX (), lineYPos + (clientTestFont.getHeight () * lineCounter)}, nextRenderLine);
		nextRenderLine = clientConsole.getLine ();
		lineCounter++;
		if (lineCounter == numLinesToRender)
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Start the console processing queue and thread
bool con_initConsole ()
//----------------------------------------------------------------------------------------------------------------------
{
//	console.setScreenSize (hiresVirtualWidth, hiresVirtualHeight);
//	console.setNumVarColumns (consoleNumColumns);
	//
	// Start the console and processing thread
	if (!clientThreads.registerMutex (CONSOLE_MUTEX_NAME))
	{
		clientMessage.message (MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT, sys_getString ("Unable to create console mutex : [ %s ]", clientThreads.getErrorString ().c_str ()));
		return false;
	}

	clientMessage.message (MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_LOGFILE, "Console started.");

//	clientConsole.addCommand ("quit", "Quit the game.", sys_shutdown);

	//
	// Variables accessible from the console
	//
//	clientConsole.addVariable ("quitLoop", VAR_TYPE_BOOL, &quitLoop);

	return true;
}

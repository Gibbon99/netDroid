//--------------------------------------------------------------------------------------------------------------------
//
// Script for client to run on startup
void clientInit()
//--------------------------------------------------------------------------------------------------------------------
{
	clientMessage.message(1, "Running clientInit from inside client script");

	clientScriptEngine.addScriptFunction("scriptOne", "script");

// Set the scripts to be loaded and run on the client
//	c_scriptAddScript("loadResources");
}

//--------------------------------------------------------------------------------------------------------------------
//
// Load the binary resources for the client
void loadClientResources()
//--------------------------------------------------------------------------------------------------------------------
{
	clientMessage.message(1, "Load client resource files.");

	c_sendRequestToServer("splash", 57);
}

//--------------------------------------------------------------------------------------------------------------------
//
// Script for client to run on startup
void clientInit()
//--------------------------------------------------------------------------------------------------------------------
{
	clientMessage.message(1, "Running clientInit from inside client script");

	clientScriptEngine.addScriptFunction("scriptOne", "loadClientResources");

	loadClientResources();

// Set the scripts to be loaded and run on the client
//	c_scriptAddScript("loadResources");
}

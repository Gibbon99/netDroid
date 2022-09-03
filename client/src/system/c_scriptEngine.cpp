#include "../../hdr/system/c_scriptEngine.h"
#include "../../hdr/main.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Print messages from script compiler to console
void c_scriptOutput (const asSMessageInfo *msg, [[maybe_unused]]void *param)
//----------------------------------------------------------------------------------------------------------------------
{
	const char *type      = "ERR ";

	if (msg->type == asMSGTYPE_WARNING)
	{
		type       = "WARN";
	}
	else if (msg->type == asMSGTYPE_INFORMATION)
	{
		type       = "INFO";
	}

	clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("%s (%d, %d) : %s : %s", msg->section, msg->row, msg->col, type, msg->message));
}


//----------------------------------------------------------------------------------------------------------------------
//
// This is how we call a script from the Host program : Name in Script : Name to call from host
bool c_initScriptFunctions ()
//----------------------------------------------------------------------------------------------------------------------
{
	if (!clientServerEngine.addScriptFunction ("void asRunTest()", "asRunTest"))
	{
		clientMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString("%s", clientServerEngine.getLastError().c_str()));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Setup all the global variables to be shared between host and scripts
bool c_initScriptVariables ()
//----------------------------------------------------------------------------------------------------------------------
{
	if (!clientServerEngine.addHostVariable ("bool quitProgram", &quitLoop))
	{
		clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("%s", clientServerEngine.getLastError ().c_str ()));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Setup all the globals functions that the scripts can call for action in the host
bool c_initClassFunctions ()
//----------------------------------------------------------------------------------------------------------------------
{
	int r;

	//
	// Pass in the messages class to the script - write output from a script
	r = clientServerEngine.scriptEngine->RegisterObjectType ("droidMessage", 0, asOBJ_REF);
	assert(r >= 0);
	r = clientServerEngine.scriptEngine->RegisterObjectBehaviour ("droidMessage", asBEHAVE_ADDREF, "void f()", asMETHOD(droidMessage, AddRef), asCALL_THISCALL);
	assert(r >= 0);
	r = clientServerEngine.scriptEngine->RegisterObjectBehaviour ("droidMessage", asBEHAVE_RELEASE, "void f()", asMETHOD(droidMessage, ReleaseRef), asCALL_THISCALL);
	assert(r >= 0);
	r = clientServerEngine.scriptEngine->RegisterObjectMethod ("droidMessage", "void message(int, string)", asMETHOD(droidMessage, message), asCALL_THISCALL);
	assert(r >= 0);
	r = clientServerEngine.scriptEngine->RegisterGlobalProperty ("droidMessage clientMessage", &clientMessage);
	assert(r >= 0);

	if (!clientServerEngine.addHostFunction ("string sys_getString (string formatIn)", asFUNCTION(sys_getString)))
	{
		clientMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString("Failed to add host based function : [ %s ].", clientServerEngine.getLastError().c_str()));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Start the scripting engine
bool c_startScriptEngine ()
//----------------------------------------------------------------------------------------------------------------------
{
	if (!clientServerEngine.init (reinterpret_cast<asFUNCTION_t>(c_scriptOutput)))
	{
		clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("Unable to start script engine : [ %s ]", clientServerEngine.getLastError ().c_str ()));
		return false;
	}
	clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("Script engine started. Version [ %s ] Options [ %s ]", clientServerEngine.getVersion ().c_str (), clientServerEngine.getBuildOptions().c_str()));

	if (!c_initScriptFunctions ())
		return false;

	if (!c_initScriptVariables ())
		return false;

	if (!c_initClassFunctions ())
		return false;

	//
	// Get script files from the server

	//io_getScriptFileNames (clientFileMapping.getfileMappedName ("scriptsDirectory"));
	//clientServerEngine.cacheFunctions();

	//clientServerEngine.run("asRunTest", "");

	return  true;
}
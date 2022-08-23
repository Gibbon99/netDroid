#include "../../hdr/system/s_scriptEngine.h"
#include "../../hdr/main.h"
#include "../../../client/hdr/main.h"
#include "../../hdr/system/s_fileBootstrap.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Print messages from script compiler to console
void s_scriptOutput (const asSMessageInfo *msg, [[maybe_unused]]void *param)
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

	serverMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("%s (%d, %d) : %s : %s", msg->section, msg->row, msg->col, type, msg->message));
}


//----------------------------------------------------------------------------------------------------------------------
//
// This is how we call a script from the Host program : Name in Script : Name to call from host
bool s_initScriptFunctions ()
//----------------------------------------------------------------------------------------------------------------------
{
	if (!serverScriptEngine.addScriptFunction ("void asRunTest()", "asRunTest"))
	{
		serverMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString("%s", serverScriptEngine.getLastError().c_str()));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Setup all the global variables to be shared between host and scripts
bool s_initScriptVariables ()
//----------------------------------------------------------------------------------------------------------------------
{
	if (!serverScriptEngine.addHostVariable ("bool quitProgram", &quitLoop))
	{
		serverMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("%s", serverScriptEngine.getLastError ().c_str ()));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Setup all the globals functions that the scripts can call for action in the host
bool s_initClassFunctions ()
//----------------------------------------------------------------------------------------------------------------------
{
	int r;

	//
	// Pass in the messages class to the script - write output from a script
	r = serverScriptEngine.scriptEngine->RegisterObjectType ("droidMessage", 0, asOBJ_REF);
	assert(r >= 0);
	r = serverScriptEngine.scriptEngine->RegisterObjectBehaviour ("droidMessage", asBEHAVE_ADDREF, "void f()", asMETHOD(droidMessage, AddRef), asCALL_THISCALL);
	assert(r >= 0);
	r = serverScriptEngine.scriptEngine->RegisterObjectBehaviour ("droidMessage", asBEHAVE_RELEASE, "void f()", asMETHOD(droidMessage, ReleaseRef), asCALL_THISCALL);
	assert(r >= 0);
	r = serverScriptEngine.scriptEngine->RegisterObjectMethod ("droidMessage", "void message(int, string)", asMETHOD(droidMessage, message), asCALL_THISCALL);
	assert(r >= 0);
	r = serverScriptEngine.scriptEngine->RegisterGlobalProperty ("droidMessage serverMessage", &serverMessage);
	assert(r >= 0);

	if (!serverScriptEngine.addHostFunction ("string sys_getString (string formatIn)", asFUNCTION(sys_getString)))
	{
		serverMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString("Failed to add host based function : [ %s ].", serverScriptEngine.getLastError().c_str()));
		return false;
	}

	if (!serverScriptEngine.addHostFunction ("bool s_cacheResource(string resourceName)", asFUNCTION(s_cacheResource)))
	{
		serverMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString("Failed to add host based function : [ %s ].", serverScriptEngine.getLastError().c_str()));
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Find the names of all the files in the scripts directory and store ready for loading
void io_getScriptFileNames (const std::string& directoryName)
//----------------------------------------------------------------------------------------------------------------------
{
	const std::filesystem::path scriptsDirectory{directoryName};
	std::string newScriptSection{};

	serverMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, "Script files to load and compile.");

	// directory_iterator can be iterated using a range-for loop
	for (auto const &dir_entry: std::filesystem::directory_iterator{scriptsDirectory})
	{
		newScriptSection = readTextFile(dir_entry.path ());
		serverScriptEngine.addScript ("Section_1", newScriptSection);
		std::cout << dir_entry.path () << '\n';
	}

	serverScriptEngine.compileModule();
}

//----------------------------------------------------------------------------------------------------------------------
//
// Start the scripting engine
bool s_startScriptEngine ()
//----------------------------------------------------------------------------------------------------------------------
{
	if (!serverScriptEngine.init (reinterpret_cast<asFUNCTION_t>(s_scriptOutput)))
	{
		serverMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("Unable to start script engine : [ %s ]", serverScriptEngine.getLastError ().c_str ()));
		return false;
	}
	serverMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("Script engine started. Version [ %s ] Options [ %s ]", serverScriptEngine.getVersion ().c_str (), serverScriptEngine.getBuildOptions().c_str()));

	if (!s_initScriptFunctions ())
		return false;

	if (!s_initScriptVariables ())
		return false;

	if (!s_initClassFunctions ())
		return false;

	io_getScriptFileNames (serverFileMapping.getfileMappedName ("scriptsDirectory"));
	serverScriptEngine.cacheFunctions();

	serverScriptEngine.run("asRunTest", "");

	return  true;
}
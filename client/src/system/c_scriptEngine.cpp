#include "../../hdr/system/c_scriptEngine.h"
#include "../../hdr/main.h"
#include "../../hdr/system/c_events.h"
#include "../../hdr/system/c_requests.h"

std::vector<_clientScripts> clientScripts{};

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
//	if (!clientScriptEngine.addScriptFunction ("void asRunTest()", "asRunTest"))
//	{
//		clientMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString("%s", clientScriptEngine.getLastError().c_str()));
//		return false;
//	}

//	if (!clientScriptEngine.addScriptFunction ("void c_scriptAddScript(string newScriptName)", "c_scriptAddScript"))
//	{
//		clientMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString("%s", clientScriptEngine.getLastError().c_str()));
//		return false;
//	}

	if (!clientScriptEngine.addScriptFunction ("void clientInit()", "clientInit"))
	{
		clientMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString("%s", clientScriptEngine.getLastError().c_str()));
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
	if (!clientScriptEngine.addHostVariable ("bool quitProgram", &quitLoop))
	{
		clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("%s", clientScriptEngine.getLastError ().c_str ()));
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
	r = clientScriptEngine.scriptEngine->RegisterObjectType ("droidMessage", 0, asOBJ_REF);
	assert(r >= 0);
	r = clientScriptEngine.scriptEngine->RegisterObjectBehaviour ("droidMessage", asBEHAVE_ADDREF, "void f()", asMETHOD(droidMessage, AddRef), asCALL_THISCALL);
	assert(r >= 0);
	r = clientScriptEngine.scriptEngine->RegisterObjectBehaviour ("droidMessage", asBEHAVE_RELEASE, "void f()", asMETHOD(droidMessage, ReleaseRef), asCALL_THISCALL);
	assert(r >= 0);
	r = clientScriptEngine.scriptEngine->RegisterObjectMethod ("droidMessage", "void message(int, string)", asMETHOD(droidMessage, message), asCALL_THISCALL);
	assert(r >= 0);
	r = clientScriptEngine.scriptEngine->RegisterGlobalProperty ("droidMessage clientMessage", &clientMessage);
	assert(r >= 0);

	r = clientScriptEngine.scriptEngine->RegisterObjectType ("droidScript", 0, asOBJ_REF);
	assert(r >= 0);
	r = clientScriptEngine.scriptEngine->RegisterObjectBehaviour ("droidScript", asBEHAVE_ADDREF, "void f()", asMETHOD(droidScript, AddRef), asCALL_THISCALL);
	assert(r >= 0);
	r = clientScriptEngine.scriptEngine->RegisterObjectBehaviour ("droidScript", asBEHAVE_RELEASE, "void f()", asMETHOD(droidScript, ReleaseRef), asCALL_THISCALL);
	assert(r >= 0);
	r = clientScriptEngine.scriptEngine->RegisterObjectMethod ("droidScript", "bool addScriptFunction(string functionName, string functionCall)", asMETHOD(droidScript, addScriptFunction), asCALL_THISCALL);
	assert(r >= 0);
	r = clientScriptEngine.scriptEngine->RegisterGlobalProperty ("droidScript clientScriptEngine", &clientScriptEngine);
	assert(r >= 0);

	if (!clientScriptEngine.addHostFunction ("string sys_getString (string formatIn)", asFUNCTION(sys_getString)))
	{
		clientMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString("Failed to add host based function : [ %s ].", clientScriptEngine.getLastError().c_str()));
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
	if (!clientScriptEngine.init (reinterpret_cast<asFUNCTION_t>(c_scriptOutput)))
	{
		clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("Unable to start script engine : [ %s ]", clientScriptEngine.getLastError ().c_str ()));
		return false;
	}
	clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("Script engine started. Version [ %s ] Options [ %s ]", clientScriptEngine.getVersion ().c_str (), clientScriptEngine.getBuildOptions().c_str()));

	if (!c_initScriptFunctions ())
		return false;

	if (!c_initScriptVariables ())
		return false;

	if (!c_initClassFunctions ())
		return false;

	printf("Script engine ready to use\n\n");

	return  true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Convert a script data packet to a script
void c_convertPacketToScript(dataPacket dataPacketIn)
//----------------------------------------------------------------------------------------------------------------------
{
	auto scriptContents = std::string(dataPacketIn.binaryData.begin(), dataPacketIn.binaryData.end());

	auto testCRC = CRC::Calculate(scriptContents.c_str(), scriptContents.size(), CRC::CRC_32());

	if (testCRC != dataPacketIn.crc)
	{
		clientMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_LOGFILE, sys_getString("CRC mismatch between script contents. Needed [ %i ] - Got [ %i ]", dataPacketIn.crc, testCRC));
		//
		// TODO: Request file again
		return;
	}

	switch (dataPacketIn.packetType)
	{
		case DATA_PACKET_TYPES::PACKET_INIT_SCRIPT:

			c_removeRequest("initScript");      // Remove the request from the queue

			clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_CONSOLE, sys_getString("Filename [ %s ]", getFilenameFromString(dataPacketIn.packetString).c_str()));
			if (!clientScriptEngine.addScript (getFilenameFromString(dataPacketIn.packetString), scriptContents))
			{
				clientMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_CONSOLE, sys_getString("Error adding a new script - [ %s ]", clientScriptEngine.getLastError().c_str()));
				return;
			}
			if (!clientScriptEngine.compileModule())
			{
				clientMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_CONSOLE, sys_getString("Error compiling script module - [ %s ]", clientScriptEngine.getLastError().c_str()));
				return;
			}

			if (!clientScriptEngine.cacheFunctions())
			{
				clientMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_CONSOLE, sys_getString("Error caching functions and creating context - [ %s ]", clientScriptEngine.getLastError().c_str()));
				return;
			}

			c_addEventToQueue (EventType::EVENT_GAME_LOOP, EventAction::ACTION_RUN_SCRIPT_FUNCTION, 0, 0, 0, 0, glm::vec2{}, glm::vec2{}, "clientInit");

			clientMessage.message (MESSAGE_TARGET_DEBUG, sys_getString ("Contents \n\n %s \n", scriptContents.c_str()));
			break;

		default:
			clientMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_CONSOLE, sys_getString("Unknown script packet type.").c_str());
			break;
	}

}
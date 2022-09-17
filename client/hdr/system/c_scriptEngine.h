#pragma once

#include "com_globals.h"

typedef struct
{
	std::string scriptName{};
	std::string scriptContents{};
} _clientScripts;

extern std::vector<_clientScripts> clientScripts;

// Start the scripting engine
bool c_startScriptEngine ();

// Convert a script data packet to a script
void c_convertPacketToScript(dataPacket dataPacketIn);
#pragma once

#include "com_globals.h"

// Start the scripting engine
bool c_startScriptEngine ();

// Convert a script data packet to a script
void c_convertPacketToScript(dataPacket dataPacketIn);
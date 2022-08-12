#pragma once

#include <fstream>
#include <iostream>
#include <filesystem>
#include "angelscript.h"
#include "com_util.h"
#include "../../hdr/system/s_resources.h"

// Print messages from script compiler to console
void s_scriptOutput(const asSMessageInfo *msg, [[maybe_unused]]void *param);

// Start the scripting engine
bool s_startScriptEngine ();
#pragma once

#include <string>
#include "com_message.h"
#include "../main.h"
#include "com_util.h"

// Log the error message and then exit
void c_shutdownWithError (const std::string &errorMessage);

// Shutdown normally - free all memory
void c_shutdown();
#pragma once

#include <string>
#include "com_message.h"
#include "../../hdr/main.h"
#include "com_util.h"

// Log the error message and then exit
void s_shutdownWithError (const std::string &errorMessage);
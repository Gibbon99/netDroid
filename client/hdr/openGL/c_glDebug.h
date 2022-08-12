#pragma once

#include <string>
#include "../../hdr/main.h"

// Return the errorNum as a readable string
std::string getGLErrorString (int errorNum);

// Get all the openGL errors
void gl_getAllGLErrors (int errorNum, const char *calledFrom, int line);

// OpenGL Debug - Register callback
void gl_registerDebugCallback ();
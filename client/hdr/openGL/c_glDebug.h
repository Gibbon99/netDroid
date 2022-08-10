#pragma once

#include "../../hdr/main.h"

// Get all the openGL errors
void gl_getAllGLErrors (int errorNum, const char *calledFrom, int line);

// OpenGL Debug - Register callback
void gl_registerDebugCallback ();
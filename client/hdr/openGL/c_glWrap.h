#pragma once

#include <string>
#include <map>

// Wrap the call to generate a buffer ID so we can print out some text with it
uint wrapglGenBuffers(int numBuffers, const std::string& bufferText);
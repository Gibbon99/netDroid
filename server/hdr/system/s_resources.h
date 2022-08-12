#pragma once

#include <map>
#include "com_binaryBlob.h"
#include "../../hdr/system/s_fileBootstrap.h"

extern std::map<std::string, droidBinaryBlob>  binaryFiles;

// Load a binary file into the array
bool s_cacheResource(const std::string &resourceName);
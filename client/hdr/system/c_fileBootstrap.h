#pragma once

#include <vector>
#include <map>
#include <string>
#include "com_fileMapping.h"
#include "com_util.h"

extern droidFileMapping    clientFileMapping;

// Find the basefile file and load in index to file mapping
bool c_initFileMapping ();
#pragma once

#include <string>
#include <sys/inotify.h>
#include "../../hdr/main.h"
#include "com_util.h"
#include "../network/s_network.h"
#include "../../hdr/system/s_shutdown.h"
#include "com_fileMapping.h"

extern droidFileMapping    serverFileMapping;

// Find the basefile file and load in index to file mapping
bool initFileMapping ();

// Setup inotify to monitor for changes to files in data directory
bool initFileWatch();

// Start a thread to run the inotify function
bool startiNotifyThread ();
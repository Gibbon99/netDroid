#pragma once

#include <string>
#include <cstdarg>
#include <iostream>
#include <iomanip>
#include <ctime>

#include "enet/enet.h"
#include "com_globals.h"

// Pass in string and parameters to format and return a string
std::string sys_getString(std::string formatIn, ...);

// Return the hostname of an IP address
std::string getHostnameFromAddress(ENetAddress whichAddress);

// Return the current time as a string
std::string sys_getTime ();

// Return the current date as a string
std::string sys_getDate ();

// Unwrap binary stream data into datapacket structure
dataPacket deSerialiseDataPacket (std::vector<uint8_t> newDataPacket, size_t dataSize);

// Break up entry line into tokens
std::vector<std::string> tokeniseLine (std::string entryLine, std::string regExpression);
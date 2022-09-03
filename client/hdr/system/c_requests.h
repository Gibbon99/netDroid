#pragma once

#include <string>
#include "com_globals.h"

struct requestType_
{
	std::string       requestString{};
	int               requestTime{};
	int               requestCount{};     // How many iimes has this been requested
	DATA_PACKET_TYPES requestType{};
};

extern std::vector<requestType_> requestQueue;

// A Request has been fulfilled - so remove it from the queue
void c_removeRequest (std::string_view requestName);

// Init the request queue - spin up a thread to monitor and reissue requests if the requestCount is too high
bool c_initRequestQueue ();

// Add a new request to the queue
void c_addRequestToQueue (std::string newRequestString, DATA_PACKET_TYPES newRequestType);
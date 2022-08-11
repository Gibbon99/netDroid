#pragma once

#include "c_events.h"
#include "vec2.hpp"

#include <string>
#include <queue>
#include <SDL_events.h>

enum myEvents
{
	GAME_LOOP_EVENT = 0,
	USER_EVENT_TEXTURE_UPLOAD
};

typedef struct
{
	unsigned int eventType;
	int          eventAction;
	int          data1;
	int          data2;
	int          data3;
	glm::vec2    vec2_1;
	glm::vec2    vec2_2;
	std::string  eventString;
} _myEventData;

extern std::queue<_myEventData> gameEventQueue;

// Create a custom event to be sent
void evt_sendEvent ( uint type, int action, int data1, int data2, int data3, const glm::vec2 vec2_1, const glm::vec2 vec2_2, std::string textString);

// Handle SDL events from main loop
void handleEvents(SDL_Event event);

// Get any network events and put onto processing queue
void getClientNetworkEvents ();

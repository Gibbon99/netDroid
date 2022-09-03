#pragma once

#include <string>
#include <queue>
#include <SDL_events.h>
#include "vec2.hpp"
#include "enet/enet.h"
#include "../../hdr/main.h"
#include "../../hdr/system/c_gameEvents.h"

enum class EventType
{
	EVENT_GAME_LOOP = 0,
	EVENT_CONSOLE
};

enum class EventAction
{
	ACTION_UPLOAD_TEXTURE = 0,
	ACTION_CONSOLE_ADD,
	ACTION_REQUEST_INIT_SCRIPT
};

struct myEventData_
{
	EventType   eventType{};
	EventAction eventAction{};
	int         data1{};
	int         data2{};
	int         data3{};
	int         delayCounter{};
	glm::vec2   vec2_1{};
	glm::vec2   vec2_2{};
	std::string eventString{};
};

extern std::queue<myEventData_> gameEventQueue;

// Create a custom event to be sent
void c_addEventToQueue (EventType type, EventAction action, int newDelayCounter, int data1, int data2, int data3, const glm::vec2 vec2_1, const glm::vec2 vec2_2, std::string textString);

// Handle SDL events from main loop
void c_handleEvents (SDL_Event event);

// Get any network events and put onto processing queue
void c_getNetworkEvents ();

#pragma once

#include <SDL_surface.h>
#include "com_globals.h"
#include "../../hdr/main.h"

extern std::map<std::string, _droidShaders> droidShaders;

// Convert a datapacket image to a surface ready for display on the client
void c_convertPacketToSurface(dataPacket newDataPacket);

bool init2DQuad (std::string newShaderName);

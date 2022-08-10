#pragma once

#include "openGL/c_glDebug.h"
#include "defines.h"
#include "GL/glew.h"
#include "glm.hpp"

#include <iostream>
#include <enet/enet.h>
#include "angelscript.h"
#include "classes/com_droidTime.h"
#include "classes/com_networkState.h"
#include "classes/c_shaders.h"
#include "com_message.h"
#include "com_globals.h"
#include "com_texture.h"
#include "network/c_network.h"
#include "../hdr/system/c_processClientFrame.h"
#include "../hdr/system/c_processNetworkState.h"
#include "../hdr/classes/c_window.h"
#include "../hdr/classes/client.h"
#include "../hdr/classes/c_glFont.h"

extern droidClient  clientNetworkObject;
extern networkState clientNetworkState;
extern droidMessage clientMessage;
extern droidWindow  clientWindow;

extern droidTexture testTexture;
extern droidGLFont  clientTestFont;

extern CSimpleIniA iniFile;

extern SDL_Texture *hex_tex;

extern bool quitLoop;
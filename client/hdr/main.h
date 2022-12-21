#pragma once

#include "GL/glew.h"
#include "openGL/c_glDebug.h"
#include "defines.h"
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
#include "com_console.h"
#include "../hdr/system/c_graphics.h"
#include "../hdr/system/c_gameEvents.h"
#include "../hdr/system/c_console.h"
#include "../hdr/classes/c_audioClass.h"
#include "com_droidScript.h"

extern droidTime          gameTime;
extern droidThreadsEngine clientThreads;
extern droidLanguage      clientLanguage;
extern droidClient        clientNetworkObject;
extern networkState       clientNetworkState;
extern droidMessage       clientMessage;
extern droidWindow        clientWindow;
extern droidGLFont        clientTestFont;
extern droidConsole       clientConsole;
extern droidAudio         clientAudio;
extern droidScript        clientScriptEngine;

extern std::map<std::string, droidTexture> clientTextures;

extern CSimpleIniA iniFile;

extern bool quitLoop;

enum class MODE_TYPES
{
	MODE_REQUEST_WAIT = 0,
	MODE_REQUEST_INIT_SCRIPT,
	MODE_WAIT_FOR_INIT_SCRIPT,
	MODE_LOAD_RESOURCES,
	MODE_SPLASH_SCREEN
};

extern MODE_TYPES currentMode;
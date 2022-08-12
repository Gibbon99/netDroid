#pragma once

#include "../hdr/classes/server.h"
#include "angelscript.h"
#include "com_message.h"
#include "com_language.h"
#include "pods.h"
#include "com_texture.h"
#include "com_binaryBlob.h"
#include "com_droidScript.h"

extern bool          quitLoop;
extern droidServer   serverObject;
extern droidMessage  serverMessage;
extern droidLanguage serverLanguage;
extern droidScript   serverScriptEngine;

extern droidBinaryBlob testBinaryFile;

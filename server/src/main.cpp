#include <iostream>
#include "../hdr/main.h"
#include "classes/com_droidTime.h"

#ifdef __WIN32__
#include "SDL.h"
#else

#include "SDL2/SDL.h"
#include "com_globals.h"
#include "../hdr/classes/s_signals.h"

#include "com_netEvents.h"
#include "../hdr/network/s_network.h"
#include "../hdr/system/s_processFrame.h"
#include "../hdr/system/s_fileBootstrap.h"
#include "com_droidScript.h"
#include "../hdr/system/s_scriptEngine.h"

#endif

droidTime     gameTime{};
droidServer   serverObject{};
droidSignals  signalHandler{};
droidMessage  serverMessage{"serverLogfile.log"};
droidLanguage serverLanguage{};
droidScript   serverScriptEngine{};

CSimpleIniA iniFile;
bool        quitLoop{false};

int         listenPort = 1234;
std::string listenAddress{"127.0.0.1"};
std::string languageFileType{"englishLangFile"};

int main (int, char **)
{

	Uint32 currentTime{0};
	Uint32 maxNumUpdateLoops{0};
	double msPerUpdate = 1000.0f / TICKS_PER_SECOND;
	double timeLag     = 0.0f;

	if (!initFileMapping ())
		s_shutdownWithError ("Unable to start the file mapping function.");

	if (!serverLanguage.loadLanguageFile (serverFileMapping.getfileMappedName (languageFileType)))
		s_shutdownWithError ("Unable to load a language file.");

	serverMessage.message (MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT, serverLanguage.getMappedString ("serverStarted"));
	//
	// Install signal handler so we can quit server
	if (!signalHandler.installHandler ())
		s_shutdownWithError (serverLanguage.getMappedString ("signalError"));
	serverMessage.message (MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT, serverLanguage.getMappedString ("signalInstalled"));
	//
	// Start listening for network events
	if (!startEventNetwork ())
		s_shutdownWithError (serverLanguage.getMappedString ("eventSystemError"));

	Uint32 previousTime = gameTime.getTicks ();

	if (!serverObject.initServer (listenAddress, 16, listenPort))
		s_shutdownWithError (serverLanguage.getMappedString ("networkLayerError"));

	serverMessage.message (MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT, sys_getString (serverLanguage.getMappedString ("serverInfo"), listenAddress.c_str (), listenPort));

	SDL_Init (SDL_INIT_EVERYTHING);

	if (!s_startScriptEngine ())
		s_shutdownWithError(serverLanguage.getMappedString("scriptStartError"));

	while (!quitLoop)
	{
		previousTime      = currentTime;
		currentTime       = gameTime.getTicks ();
		maxNumUpdateLoops = 0;

		timeLag += (currentTime - previousTime);

		while (timeLag >= msPerUpdate && maxNumUpdateLoops < MAX_FRAMESKIP)
		{
			processFrame ();
			timeLag -= msPerUpdate;
//            thinkFPS++;
			maxNumUpdateLoops++;
		}
		SDL_Delay (30);         // Simulate screen render delay
	}
	serverObject.destroyServer ();
}

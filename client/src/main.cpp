#include "../hdr/main.h"
#include "../hdr/system/c_renderClientFrame.h"
#include "../hdr/system/c_fileBootstrap.h"
#include "../hdr/system/c_shutdown.h"
#include "com_language.h"
#include "../hdr/system/c_graphics.h"
#include "../hdr/system/c_gameEvents.h"

droidTime     gameTime{};
droidClient   clientNetworkObject{};
droidMessage  clientMessage{"clientLogfile.log"};
droidWindow   clientWindow (800, 600);
droidLanguage clientLanguage{};
networkState  clientNetworkState{};
droidGLFont   clientTestFont{};

std::map<std::string, droidTexture> clientTextures{};

droidTexture testTexture{};

CSimpleIniA iniFile{};

int __gl_error_code;
bool quitLoop = false;

//----------------------------------------------------------------------------------------------------------------------
//
// Entry Point
int main (int, char **)
//----------------------------------------------------------------------------------------------------------------------
{
	Uint32 currentTime{0};
	Uint32 maxNumUpdateLoops{0};
	double msPerUpdate = 1000.0f / TICKS_PER_SECOND;
	double timeLag     = 0.0f;

	if (!c_initFileMapping ())
		c_shutdownWithError ("Unable to start the file mapping function.");

	if (!clientLanguage.loadLanguageFile (clientFileMapping.getfileMappedName ("englishLangFile")))
		c_shutdownWithError ("Unable to load a language file.");

	clientMessage.message (MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_STD_OUT, sys_getString (clientLanguage.getMappedString ("clientStarted"), "Test"));

	if (!clientWindow.create ("netDroid", false))
	{
		clientMessage.message (MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT, sys_getString (clientLanguage.getMappedString ("clientUnableCreateWindow"), clientWindow.getLastError ().c_str ()));
		return -1;
	}
	gl_registerDebugCallback ();
	clientWindow.setWindowTitle (clientLanguage.getMappedString ("clientWindowTitle"));

	clientMessage.message (MESSAGE_TARGET_STD_OUT, sys_getString ("%s", clientWindow.getCompiledVersion ().c_str ()));
	clientMessage.message (MESSAGE_TARGET_STD_OUT, sys_getString ("%s", clientWindow.getLinkedVersion ().c_str ()));

	if (!c_createGameLoopMutex())
		return -1;

	c_startNetworkMonitor ();
	startNetworkStateThread ();
	Uint32 previousTime = gameTime.getTicks ();

	if (!clientTestFont.init ("Digital.ttf", 20, glm::vec2{128,128}))
	{
		clientMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString("%s", clientTestFont.returnLastError().c_str()));
		return -1;
	}

	SDL_Delay (500);    // Give the network stack time to start

	clientNetworkState.setNewState (NETWORK_STATE_TRY_SERVER_CONNECT);

	init2DQuad ("2DQuad");

	float frameTime, frameStart;

	while (!quitLoop)
	{
		frameStart = gameTime.getTicks();

		previousTime      = currentTime;
		currentTime       = gameTime.getTicks ();
		maxNumUpdateLoops = 0;

		timeLag += (currentTime - previousTime);

		while (timeLag >= msPerUpdate && maxNumUpdateLoops < MAX_FRAMESKIP)
		{
			processClientFrame ();
			timeLag -= msPerUpdate;
            //thinkFPS++;
			maxNumUpdateLoops++;
		}
		clientTestFont.addText (glm::vec4{0.5f, 0.5f, 0.6f, 1.0f}, glm::vec2{clientWindow.getWidth() - clientTestFont.getTextWidth("Frametime [ 88.88 ]"),
																			 clientWindow.getHeight() - clientTestFont.getHeight()}, sys_getString("Frametime [ %2.2f ]", frameTime));
		renderFrame ();

		frameTime = gameTime.getTicks() - frameStart;
	}

	clientNetworkObject.destroyClient ();
}

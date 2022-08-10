#include <cstdlib>
#include <iostream>
#include "../../hdr/classes/s_signals.h"
#include "../../hdr/main.h"
#include "com_util.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Call this when a signal is caught - shutdown server nicely
// TODO - Hook up to exit function
//----------------------------------------------------------------------------------------------------------------------
#ifdef __WIN32__
BOOL WINAPI ConsoleHandler(DWORD dwType)
{
	switch (dwType)
	{
		case CTRL_C_EVENT:
			break;

		case CTRL_BREAK_EVENT:
			break;
	}
	return TRUE;
}
#else

void signalHandler (int signum)
{
	serverMessage.message(MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_LOGFILE, sys_getString("Interrupt signal [ %i ] received.", signum));
	exit (signum);
}

#endif

//----------------------------------------------------------------------------------------------------------------------
//
// Install a cross platform signal handler
bool droidSignals::installHandler ()
//----------------------------------------------------------------------------------------------------------------------
{
#ifdef __WIN32__
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE))
		return false;
	return true;
#else
	signal (SIGINT, signalHandler);
	signal (SIGTERM, signalHandler);
	return true;
#endif
}

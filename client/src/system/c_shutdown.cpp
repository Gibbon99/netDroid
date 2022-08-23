#include "../../hdr/system/c_shutdown.h"

//--------------------------------------------------------------------------------------------------------------------
//
// Log the error message and then exit
void c_shutdownWithError (const std::string &errorMessage)
//--------------------------------------------------------------------------------------------------------------------
{
	clientMessage.message (MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT, sys_getString("FATAL ERROR : %s", errorMessage.c_str()));
	exit (-1);
}

//--------------------------------------------------------------------------------------------------------------------
//
// Shutdown normally - free all memory
void c_shutdown()
//--------------------------------------------------------------------------------------------------------------------
{
	clientNetworkObject.destroyClient ();
	clientThreads.stopThreads();
	clientThreads.destroyMutexes();
}

#include "../../hdr/system/s_shutdown.h"

//--------------------------------------------------------------------------------------------------------------------
//
// Log the error message and then exit
void s_shutdownWithError (const std::string &errorMessage)
//--------------------------------------------------------------------------------------------------------------------
{
	serverMessage.message (MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT, sys_getString("FATAL ERROR : %s", errorMessage.c_str()));
	exit (-1);
}



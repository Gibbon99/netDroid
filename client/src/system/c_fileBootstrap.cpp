#include "../../hdr/system/c_fileBootstrap.h"
#include "../../hdr/main.h"

std::vector<std::string> fileBootstrapNames{"./client/fileBootstrap.ini", "data/fileBootstrap.ini", "build/fileBootstrap.ini"};
droidFileMapping         clientFileMapping{};

//--------------------------------------------------------------------------------------------------------------------
//
// Find the basefile file and load in index to file mapping
bool c_initFileMapping ()
//--------------------------------------------------------------------------------------------------------------------
{
	if (!clientFileMapping.getBootstrapFilename (fileBootstrapNames))
	{
		clientMessage.message (MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT, sys_getString ("%s", clientFileMapping.getLastError ().c_str ()));
		return false;
	}

	if (!clientFileMapping.getFileMappingsFromFile ())
	{
		clientMessage.message (MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT, sys_getString ("%s", clientFileMapping.getLastError ().c_str ()));
		return false;
	}

	return true;
}







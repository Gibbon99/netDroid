#include "../../hdr/system/s_resources.h"

std::map<std::string, droidBinaryBlob>  binaryFiles;

//----------------------------------------------------------------------------------------------------------------------
//
// Load a binary file into the array
bool s_cacheResource(const std::string &resourceName)
//----------------------------------------------------------------------------------------------------------------------
{
	droidBinaryBlob tempBinaryBlob;

	binaryFiles.insert (std::pair<std::string, droidBinaryBlob> (resourceName, tempBinaryBlob));
	if (!binaryFiles[resourceName].loadFromDisk (serverFileMapping.getfileMappedName (resourceName)))
	{
		serverMessage.message(MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString("Unable to load [ %s ] : [ %s ]", resourceName.c_str(), binaryFiles[resourceName].getLastError().c_str()));
		return false;
	}
	serverMessage.message(MESSAGE_TARGET_LOGFILE, sys_getString("Loaded file [ %s ] : [ %i ]", resourceName.c_str(), binaryFiles[resourceName].getBlobSize()));
	return true;
}
//----------------------------------------------------------------------------------------------------------------------
//
// Load all the resources into memory ready to send to clients
bool s_cacheAllResources()
//----------------------------------------------------------------------------------------------------------------------
{

	return true;
}
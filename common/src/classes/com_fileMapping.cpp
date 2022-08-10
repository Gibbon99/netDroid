#include "com_fileMapping.h"
#include "com_util.h"

//--------------------------------------------------------------------------------------------------------------------
//
// Return the last error message
std::string droidFileMapping::getLastError ()
//--------------------------------------------------------------------------------------------------------------------
{
	return lastError;
}

//--------------------------------------------------------------------------------------------------------------------
//
// Get the filename from an index
std::string droidFileMapping::getfileMappedName (const std::string &indexKey)
//--------------------------------------------------------------------------------------------------------------------
{
	auto search = fileMapping.find (indexKey);
	if (search != fileMapping.end ())
	{
		return search->second;
	}
	else
	{
		lastError = sys_getString("File map [ %s ] not found.", indexKey.c_str());
		return "";
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// Load the data from the bootStrap file and put values into std::map
// This is a lookup table for which files to load for which function
bool droidFileMapping::getFileMappingsFromFile ()
//--------------------------------------------------------------------------------------------------------------------
{
	std::string              line{};
	std::ifstream            mappingFile{};
	std::vector<std::string> mappingFileTokens;

	mappingFile.open (fileBootstrapFilename, std::ios_base::in);
	if (!mappingFile.is_open ())
	{
		lastError = sys_getString ("Unable to open mapping file [ %s ]\n", fileBootstrapFilename.c_str ());
		return false;
	}

	while (getline (mappingFile, line))
	{
		mappingFileTokens = tokeniseLine (line, "#");
		fileMapping.emplace (std::pair<std::string, std::string> (mappingFileTokens[0], mappingFileTokens[1]));
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------
//
// Read the bootstrap filename from config ini file
// Search through list of filenames until it's found, or error if not found at all
bool droidFileMapping::getBootstrapFilename (const std::vector<std::string> &filenameList)
//--------------------------------------------------------------------------------------------------------------------
{
	//
	// Check if this has been run before - if so, clear out data
	if (!fileBootstrapFilename.empty())
	{
		fileBootstrapFilename = "";
		fileMapping.clear();
	}

	for (const auto &bootstrapFilename: filenameList)
	{
		auto returnCode = iniFile.LoadFile (bootstrapFilename.c_str ());
		if (returnCode == SI_OK)
		{
			auto returnValue      = iniFile.GetValue ("Main", "bootstrapFilename", "droidDefault");
			if (strcmp (returnValue, "droidDefault") == 0)
			{
				lastError = sys_getString ("Unable to locate value [ %s ] in config file.", "bootstrapFilename");
				return false;
			}
			fileBootstrapFilename = returnValue;

			getFileMappingsFromFile ();

			return true;
		}
	}
	lastError = "Unable to find bootStrap file. Bad installation or missing file.";
	return false;
}
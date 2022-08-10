#pragma once

#include <vector>
#include <string>
#include <fstream>
#include "simpleIni.h"

class droidFileMapping
{

public:
	bool getBootstrapFilename (const std::vector<std::string> &filenameList);

	bool getFileMappingsFromFile ();

	std::string getfileMappedName (const std::string &indexKey);

	std::string getLastError ();

private:
	CSimpleIniA                        iniFile{};
	std::string                        lastError{};
	std::string                        fileBootstrapFilename{};
	std::map<std::string, std::string> fileMapping{};
};

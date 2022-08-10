#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>

class droidLanguage
{
public:

	std::string getLastError ();

	bool loadLanguageFile (const std::string &fileName);

	std::string getMappedString (const std::string &indexKey);

private:
	std::string                        lastError{};
	std::map<std::string, std::string> languageMapping{};
};


#include "com_language.h"
#include "com_util.h"

//--------------------------------------------------------------------------------------------------------------------
//
// Return the held last error message
std::string droidLanguage::getLastError()
//--------------------------------------------------------------------------------------------------------------------
{
	return lastError;
}

//--------------------------------------------------------------------------------------------------------------------
//
// Load a language fie into the mapping array
bool droidLanguage::loadLanguageFile(const std::string &fileName)
//--------------------------------------------------------------------------------------------------------------------
{
	std::string              line{};
	std::ifstream            languageFile{};
	std::vector<std::string> mappingLanguageTokens;

	//
	// If there is already a language loaded, then clear it and load the new one in its place
	if (!languageMapping.empty())
	{
		languageMapping.clear();
	}

	languageFile.open(fileName, std::ios_base::in);
	if (!languageFile.is_open())
	{
		lastError = sys_getString ("Unable to open language file [ %s ]", fileName.c_str());
		return false;
	}

	// TODO : Error check strings coming in - empty or not ??

	while (getline (languageFile, line))
	{
		mappingLanguageTokens = tokeniseLine (line, "#");
		languageMapping.emplace (std::pair<std::string, std::string> (mappingLanguageTokens[0], mappingLanguageTokens[1]));
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------
//
// Get the language string from an index
std::string droidLanguage::getMappedString (const std::string &indexKey)
//--------------------------------------------------------------------------------------------------------------------
{
	auto search = languageMapping.find (indexKey);
	if (search != languageMapping.end ())
	{
		return search->second;
	}
	else
	{
		return sys_getString("Language map [ %s ] not found.", indexKey.c_str());
	}
}

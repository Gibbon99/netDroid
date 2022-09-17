#include "../../hdr/system/c_scriptData.h"
#include "../../hdr/system/c_scriptEngine.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Add a new script filename to the vector of scripts to load
void c_scriptAddScript(std::string newScriptName)
//----------------------------------------------------------------------------------------------------------------------
{
	_clientScripts tempScriptName{};

	tempScriptName.scriptName = newScriptName;

	clientScripts.push_back(tempScriptName);
}

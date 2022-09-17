#pragma once

#define MODULE_NAME                 "clientModule"

#include "../script/angelscript.h"
#include "../script/scriptbuilder.h"
#include "../script/scriptstdstring.h"

typedef int (*functionPtr) (...);

typedef void      (*scriptFunctionPtrStr) (int, std::string);

class droidScript
{
public:

	void AddRef();
	void ReleaseRef();

	bool init (asFUNCTION_t scriptOutputFunction);

	std::string getScriptError (int errNo);
	std::string getLastError();
	std::string getVersion();
	std::string getBuildOptions();

	bool addScriptFunction(const std::string &funcName, std::string hostCallName);
	bool addHostVariable(const std::string &varName, void *varPtr);
	bool addHostFunction(const std::string &funcName, asSFuncPtr funcPtr);

	bool compileModule();
	bool addScript (const std::string &scriptSectionName, const std::string &newScriptCode);
	bool cacheFunctions();
	bool run(const std::string &functionName, const std::string &param);

	asIScriptEngine *scriptEngine = nullptr;

private:

	scriptFunctionPtrStr funcOutput{};
	std::string lastError{};

//----------------------------------------------------------------------------------------------------------------------
//
// AngelScript engine
//
//----------------------------------------------------------------------------------------------------------------------

	asIScriptContext *context            = nullptr;
	CScriptBuilder   builder;
	bool             scriptEngineRunning = false;

//----------------------------------------------------------------------------------------------------------------------
//
// Structure to hold all the functions in the script
//
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
//
// This is used to hold the function ID and is used when creating functions
// from within a running script. A copy of scriptFunctionName is used as
// the source data.
//
//----------------------------------------------------------------------------------------------------------------------

	typedef struct
	{
		asIScriptFunction *funcID;
		bool              fromScript;       // Is this created from a console script
		std::string       functionName;
		std::string       scriptName;
		bool              param1;           // Does this function need something passed to it
	}                _scriptFunctionName;

	std::vector<_scriptFunctionName>  scriptFunctions;

//----------------------------------------------------------------------------------------------------------------------
//
// This is used to hold the variables that are shared between the host and script program
//
//----------------------------------------------------------------------------------------------------------------------

	struct _hostScriptFunctions
	{
		std::string scriptFunctionName{};
		asSFuncPtr  hostFunctionPtr;
	};

	struct _hostScriptVariables
	{
		std::string scriptFunctionName{};
		void        *hostFunctionPtr = nullptr;
	};
	std::vector<_hostScriptVariables> hostVariables;

//----------------------------------------------------------------------------------------------------------------------
//
// Struct to hold all the script files found in the scripts directory
//
//----------------------------------------------------------------------------------------------------------------------

	typedef struct
	{
		std::string sectionName;
		std::string scriptContents;
	}                                 _scriptFileCache;

	std::vector<_scriptFileCache> scriptFileCache;

//----------------------------------------------------------------------------------------------------------------------
//
// Struct to hold host function mapping to script function names
//
//----------------------------------------------------------------------------------------------------------------------

	std::vector<_scriptFunctionName>  scriptFunctionName;
	std::vector<_hostScriptFunctions> hostScriptFunctions;
};
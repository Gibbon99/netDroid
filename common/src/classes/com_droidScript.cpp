#include "com_droidScript.h"
#include "com_util.h"

void droidScript::AddRef()
{

}

void droidScript::ReleaseRef()
{

}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the last error string
std::string droidScript::getLastError ()
//----------------------------------------------------------------------------------------------------------------------
{
	return lastError;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return what options are compiled into the script library
std::string droidScript::getBuildOptions ()
//----------------------------------------------------------------------------------------------------------------------
{
	if (scriptEngineRunning)
		return asGetLibraryOptions ();
	else
		return "Script engine not running. Unable to supply build options.";
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the version of the script engine
std::string droidScript::getVersion ()
//----------------------------------------------------------------------------------------------------------------------
{
	if (scriptEngineRunning)
		return asGetLibraryVersion ();
	else
		return "Script engine not running. Version request not available.";
}

//----------------------------------------------------------------------------------------------------------------------
//
// Start the script engine
bool droidScript::init (asFUNCTION_t scriptOutputFunction)
//----------------------------------------------------------------------------------------------------------------------
{
	scriptEngineRunning = false;

	scriptEngine = asCreateScriptEngine (ANGELSCRIPT_VERSION);
	if (nullptr == scriptEngine)
	{
		scriptEngineRunning = false;
		lastError           = sys_getString ("Script: Error: Failed to create script engine.");
		return scriptEngineRunning;
	}

	// The script compiler will write any compiler messages to the callback.
	scriptEngine->SetMessageCallback (asFUNCTION(scriptOutputFunction), nullptr, asCALL_CDECL);

	RegisterStdString (scriptEngine);

	scriptEngineRunning = true;
	return scriptEngineRunning;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Error codes for AngelScript
std::string droidScript::getScriptError (int errNo)
//----------------------------------------------------------------------------------------------------------------------
{
	switch (errNo)
	{
		case 0:
			return ("asSUCCESS");
			break;

		case -1:
			return ("asERROR");
			break;

		case -2:
			return ("asCONTEXT_ACTIVE");
			break;

		case -3:
			return ("asCONTEXT_NOT_FINISHED");
			break;

		case -4:
			return ("asCONTEXT_NOT_PREPARED");
			break;

		case -5:
			return ("asINVALID_ARG");
			break;

		case -6:
			return ("asNO_FUNCTION");
			break;

		case -7:
			return ("asNOT_SUPPORTED");
			break;

		case -8:
			return ("asINVALID_NAME");
			break;

		case -9:
			return ("asNAME_TAKEN");
			break;

		case -10:
			return ("asINVALID_DECLARATION");
			break;

		case -11:
			return ("asINVALID_OBJECT");
			break;

		case -12:
			return ("asINVALID_TYPE");
			break;

		case -13:
			return ("asALREADY_REGISTERED");
			break;

		case -14:
			return ("asMULTIPLE_FUNCTIONS");
			break;

		case -15:
			return ("asNO_MODULE");
			break;

		case -16:
			return ("asNO_GLOBAL_VAR");
			break;

		case -17:
			return ("asINVALID_CONFIGURATION");
			break;

		case -18:
			return ("asINVALID_INTERFACE");
			break;

		case -19:
			return ("asCANT_BIND_ALL_FUNCTIONS");
			break;

		case -20:
			return ("asLOWER_ARRAY_DIMENSION_NOT_REGISTERED");
			break;

		case -21:
			return ("asAPP_CANT_INTERFACE_DEFAULT_ARRAY");
			break;

		default:
			return ("Unknown error type.");
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Add a new function that is defined in a script - create name to call it from host
bool droidScript::addScriptFunction (const std::string &funcName, std::string hostCallName)
//----------------------------------------------------------------------------------------------------------------------
{
	_scriptFunctionName tempFunctionName;

	for (const auto &funcItr: scriptFunctionName)
	{
		if (funcItr.functionName == funcName)
		{
			lastError = sys_getString ("Function name [ %s ] has already been added. Ignoring.", funcName.c_str ());
			return false;
		}
	}

	tempFunctionName.fromScript   = true;
	tempFunctionName.funcID       = nullptr;
	tempFunctionName.functionName = funcName;
	tempFunctionName.param1       = false;
	tempFunctionName.scriptName   = std::move (hostCallName);

	scriptFunctionName.push_back (tempFunctionName);

	lastError = "";
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Add a variable to the array holding all the script accessible variables
bool droidScript::addHostVariable (const std::string &varName, void *varPtr)
//----------------------------------------------------------------------------------------------------------------------
{
	_hostScriptVariables tempVar;

	tempVar.scriptFunctionName = varName;
	tempVar.hostFunctionPtr    = varPtr;

	for (const auto &varItr: hostVariables)
	{
		if (varItr.scriptFunctionName == varName)
		{
			lastError = sys_getString ("Variable [ %s ] has already been added.", varName.c_str ());
			return false;
		}
	}

	if (scriptEngine->RegisterGlobalProperty (varName.c_str (), (void *) varPtr) < 0)
	{
		lastError = sys_getString ("Script: Error: Couldn't register variable - [ %s ]", varName.c_str ());
		return false;
	}
	hostVariables.push_back (tempVar);

	lastError = "";
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Add script files into a single module
bool droidScript::addScript (const std::string &scriptSectionName, const std::string &newScriptCode)
//----------------------------------------------------------------------------------------------------------------------
{
	auto retCode = builder.StartNewModule (scriptEngine, MODULE_NAME);

	if (retCode < 0)
	{
		lastError = sys_getString ("Failed to start script builder module.");
		return false;
	}

	retCode = builder.AddSectionFromMemory (scriptSectionName.c_str (), newScriptCode.c_str (), newScriptCode.size (), 0);
	switch (retCode)
	{
		case 0:
			lastError = sys_getString ("Section with same name has already been added [ %s ]. Ignoring.", scriptSectionName.c_str ());
			return false;
			break;

		case 1: // Section added
			return true;
			break;

		default:
			lastError = sys_getString ("Failed to add script section [ %s ].", scriptSectionName.c_str ());
			return false;
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Now compile the scripts into a module
bool droidScript::compileModule()
//----------------------------------------------------------------------------------------------------------------------
{
	//
	// Build the script from the loaded sections using ScriptBuilder
	//
	auto retCode = builder.BuildModule ();
	if (retCode < 0)
	{
		lastError = sys_getString ("Failed to build the script module.");
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Execute a script function from the host
bool droidScript::run(const std::string &functionName, const std::string &param)
//----------------------------------------------------------------------------------------------------------------------
{
	int returnCode = 0;
	int testInt    = 0;

	if (!scriptEngineRunning)
	{
		lastError = sys_getString("Script engine is not running. Can not execute script function [ %s ].", functionName.c_str());
		return false;
	}

	if (nullptr == context)
	{
		lastError = sys_getString("Script context is null. Can not execute script function [ %s ].", functionName.c_str());
		return false;
	}

	for (const auto &funcItr: scriptFunctions)
	{
		if (scriptEngine != context->GetEngine ())
		{
			lastError = sys_getString ("Context DOES NOT belong to this engine. Can not execute function [ %s ].", functionName.c_str());
			return false;
		}

		if (funcItr.scriptName == functionName)
		{
			if (scriptEngine != funcItr.funcID->GetEngine ())
			{
				lastError = sys_getString ("Function [ %s ] DOES NOT belong to this engine.", functionName.c_str());
				return false;
			}

			context->Prepare (funcItr.funcID);
			//
			// Is there a parameter being passed for this function
			if (!param.empty ()) // TODO - Tokenize the parameter list and loop for all parameters
			{
				//
				// See if it's a number or not
				if (isdigit (param.c_str ()[0]))
				{
					//
					// Parameter is a number - convert before passing it in
					testInt = std::stoi (param, nullptr, 10);
					context->SetArgAddress (0, &testInt);
				}
				else
				{
					//
					// Send in a string
//					context->SetArgAddress (0, (void *) &param);
					context->SetArgAddress (0, static_cast<void *>(new std::string (param)));
				}
			}
#ifdef MY_DEBUG_SCRIPT
			std::cout << sys_getString ("Context state : [ %s ] ", getContextState (context->GetState ()).c_str ()) << std::endl;
			std::cout << sys_getString ("Execute [ %s ]", functionName.c_str ()) << std::endl;
#endif
			if (asEXECUTION_PREPARED != context->GetState ())
			{
				lastError = sys_getString ("The context is not ready to execute the function [ %s ].", functionName.c_str ());
				return false;
			}

//			while (asEXECUTION_ACTIVE == context->GetState());

			returnCode = context->Execute ();
			if (returnCode != asEXECUTION_FINISHED)
			{
				//
				// The execution didn't finish as we had planned. Determine why.
				//
				if (returnCode == asEXECUTION_ABORTED)
				{
					lastError = sys_getString ("The script was aborted before it could finish. Probably it timed out. Function [ %s ].", functionName.c_str());
					return false;
				}
				else if (returnCode == asEXECUTION_EXCEPTION)
				{
					//
					// Write some information about the script exception
					//
					asIScriptFunction *func = context->GetExceptionFunction ();
					lastError = sys_getString ("Script: The script ended with an exception.\n. Func: [ %s ]\n Module [ %s ]\n Section [ %s ] \n Line [ %i ]\n Desc [ %s ]\n",
											   func->GetDeclaration (), func->GetModuleName (), func->GetScriptSectionName (), context->GetExceptionLineNumber (), context->GetExceptionString ());
					return false;
				}
				else
				{
					lastError = sys_getString ("The script ended for an unknown reason [ %i ] - [ %s ].", returnCode, getScriptError (returnCode).c_str ());
				}
			}
			return true;
		}
	}
	lastError = sys_getString ("Function [ %s ] not found in script", functionName.c_str ());
	return false;
}


//----------------------------------------------------------------------------------------------------------------------
//
// Register all the functions to make available to the script
bool droidScript::addHostFunction(const std::string &funcName, asSFuncPtr funcPtr)
//----------------------------------------------------------------------------------------------------------------------
{
	int            returnCode;
	static asDWORD callType    = -1;
	static bool    callTypeSet = false;

	_hostScriptFunctions tempFunc;

	for (const auto &scriptItr: hostScriptFunctions)
	{
		if (scriptItr.scriptFunctionName == funcName)
		{
			lastError = sys_getString ("Function [ %s ] has already been added.", funcName.c_str ());
			return false;
		}
	}

	if (!callTypeSet)
	{
		if (!strstr (asGetLibraryOptions (), "AS_MAX_PORTABILTY"))
		{
			callType = asCALL_CDECL;
		}
		else
		{
			callType = asCALL_GENERIC;
		}
		callTypeSet = true;
	}

	tempFunc.scriptFunctionName = funcName;
	tempFunc.hostFunctionPtr    = asSFuncPtr (funcPtr);

	returnCode = scriptEngine->RegisterGlobalFunction (funcName.c_str (), (asSFuncPtr &&) funcPtr, callType); //asCALL_CDECL);
	if (returnCode < 0)
	{
		lastError = sys_getString ("Failed to registerGlobalFunction [ %s ] - [ %s ]", funcName.c_str (), getScriptError (returnCode).c_str ());
		return false;
	}
	hostScriptFunctions.push_back (tempFunc);
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Cache the functionID from functions in the scripts
bool droidScript::cacheFunctions()
//----------------------------------------------------------------------------------------------------------------------
{
	_scriptFunctionName tempFunctionName;

	auto mod = scriptEngine->GetModule (MODULE_NAME);

	//
	// Get function ID's for each function we will call in the script
	//
	for (const auto &funcItr: scriptFunctionName)
	{
		tempFunctionName.funcID = mod->GetFunctionByDecl (funcItr.functionName.c_str ());

		if (tempFunctionName.funcID == nullptr)
		{
			lastError = sys_getString ("Failed to get function ID for [ %s ]", funcItr.functionName.c_str ());
			return false;
		}
		else
		{
			//
			// Store the funcID and name to execute
			//
			tempFunctionName.param1       = funcItr.param1;
			tempFunctionName.functionName = funcItr.functionName;
			tempFunctionName.scriptName   = funcItr.scriptName;
			tempFunctionName.fromScript   = funcItr.fromScript;

			printf("Script function name [ %s ]", tempFunctionName.functionName.c_str());

			scriptFunctions.push_back (tempFunctionName);
		}
	}
	//
	// Do some preparation before execution
	context = scriptEngine->CreateContext ();

	scriptEngineRunning = true;

	return true;
}
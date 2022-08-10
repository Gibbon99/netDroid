#include "com_droidScript.h"
#include "com_util.h"


//----------------------------------------------------------------------------------------------------------------------
//
// Start the script engine
bool droidScript::init(scriptFunctionPtrStr outputFunction, asFUNCTION_t scriptOutputFunction)
//----------------------------------------------------------------------------------------------------------------------
{
	scriptEngineRunning = false;

	funcOutput = outputFunction;

	scriptEngine = asCreateScriptEngine (ANGELSCRIPT_VERSION);

	if (nullptr == scriptEngine)
	{
		scriptEngineRunning = false;
		funcOutput (-1, sys_getString ("Script: Error: Failed to create script engine- [ %s ]", getScriptError (0).c_str ()));
		scriptEngineRunning =  false;
		return scriptEngineRunning;
	}

	// The script compiler will write any compiler messages to the callback.
	scriptEngine->SetMessageCallback (asFUNCTION(scriptOutputFunction), nullptr, asCALL_CDECL);

	funcOutput (-1, sys_getString ("Script: Script Engine started."));

	RegisterStdString (scriptEngine);

	// What version are we running
	funcOutput (-1, sys_getString ("Script: Script Engine version - [ %s ]", asGetLibraryVersion ()));

	// What options are compiled
	funcOutput (-1, sys_getString ("Script: Options - [ %s ]", asGetLibraryOptions ()));

	scriptEngineRunning = true;

	return scriptEngineRunning;
}


//----------------------------------------------------------------------------------------------------------------------
//
// Error codes for AngelScript
std::string droidScript::getScriptError(int errNo)
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

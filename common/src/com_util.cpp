#include "com_util.h"
#include <vector>
#include <com_globals.h>
#include <regex>
#include <GL/glew.h>
#include <fstream>

//----------------------------------------------------------------------------------------------------------------------
//
// Pass in string and parameters to format and return a string
std::string sys_getString (std::string formatIn, ...)
//----------------------------------------------------------------------------------------------------------------------
{
	std::string s{};

	va_list args, args2;
	va_start(args, formatIn);
	va_copy(args2, args);
	//
	// Get string length
	s.resize (vsnprintf (nullptr, 0, formatIn.c_str (), args2) + 1);
	va_end(args2);
	//
	// Actually write the string and args
	vsprintf (s.data (), formatIn.c_str (), args);
	va_end(args);
	s.pop_back ();
	//
	// Return the full string
	return s;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the hostname of an IP address
std::string getHostnameFromAddress (ENetAddress whichAddress)
//----------------------------------------------------------------------------------------------------------------------
{
	std::string hostNameReturn{};
	char        hostName[128];

	if (whichAddress.host != 0)
		enet_address_get_host (&whichAddress, reinterpret_cast<char *>(&hostName), 128);
	else
		printf ("whichAddress is null\n");

	hostNameReturn = hostName;

	return hostNameReturn;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the current time as a string
std::string sys_getTime ()
//----------------------------------------------------------------------------------------------------------------------
{
	auto t  = std::time (nullptr);
	auto tm = *std::localtime (&t);

	std::ostringstream oss;
//	oss << std::put_time (&tm, "%d-%m-%Y %H-%M-%S");
	oss << std::put_time (&tm, "%H:%M:%S");
	auto str = oss.str ();

	return str;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the current date as a string
std::string sys_getDate ()
//----------------------------------------------------------------------------------------------------------------------
{
	auto t  = std::time (nullptr);
	auto tm = *std::localtime (&t);

	std::ostringstream oss;
	oss << std::put_time (&tm, "%d-%m-%Y");
	auto str = oss.str ();

	return str;
}

//--------------------------------------------------------------------------------------------------------------------
//
// Break up entry line into tokens
//
// Removes entries that are empty
std::vector<std::string> tokeniseLine (std::string entryLine, std::string regExpression)
//--------------------------------------------------------------------------------------------------------------------
{
	//
	// https://stackoverflow.com/a/27468529/4621054
	std::regex reg (regExpression);

	std::sregex_token_iterator iter (entryLine.begin (), entryLine.end (), reg, -1);
	std::sregex_token_iterator end;

	std::vector<std::string> enteredCommand (iter, end);

	enteredCommand.erase (std::remove_if (enteredCommand.begin (), enteredCommand.end (), [] (
			const std::basic_string<char> &x)
			{
			return x.empty ();
			}
	), enteredCommand.end ());

	return enteredCommand;
}

//--------------------------------------------------------------------------------------------------------------------
//
// Return the errorNum as a readable string
std::string getGLErrorString (int errorNum)
//--------------------------------------------------------------------------------------------------------------------
{
	switch (errorNum)
	{
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
			break;

		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE: A numeric argument is out of range.";
			break;

		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
			break;

		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
			break;

		case GL_STACK_UNDERFLOW:
			return "An attempt has been made to perform an operation that would cause an internal stack to underflow.";
			break;

		case GL_STACK_OVERFLOW:
			return "An attempt has been made to perform an operation that would cause an internal stack to overflow";
			break;

		default:
			return "Unknown error.";
			break;
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// Read a text file from the filesystem into a std::string
std::string readTextFile(const std::string &fileName)
//--------------------------------------------------------------------------------------------------------------------
{
	// TODO - Error checking
	//
	// open the file:
	std::ifstream file(fileName);
	//
	// Get the size of the input file
	file.seekg (0, std::ios::end);
	std::streampos fileSize = file.tellg ();
	file.seekg (0, std::ios::beg);
	//
	// read the file into the string:
	std::string fileContents;
	fileContents.resize(fileSize);
	file.read (&fileContents[0], fileSize);

	return fileContents;
}

//--------------------------------------------------------------------------------------------------------------------
//
// Return string representing the current state of the peer
std::string getPeerState(int currentState)
//--------------------------------------------------------------------------------------------------------------------
{

	switch (currentState)
	{
		case ENET_PEER_STATE_DISCONNECTED:
			return "ENET_PEER_STATE_DISCONNECTED";
			break;

		case ENET_PEER_STATE_CONNECTING:
			return "ENET_PEER_STATE_CONNECTING";
			break;

		case ENET_PEER_STATE_ACKNOWLEDGING_CONNECT:
			return "ENET_PEER_STATE_ACKNOWLEDGING_CONNECT";
			break;

		case ENET_PEER_STATE_CONNECTION_PENDING:
			return "ENET_PEER_STATE_CONNECTION_PENDING";
			break;

		case ENET_PEER_STATE_CONNECTION_SUCCEEDED:
			return "ENET_PEER_STATE_CONNECTION_SUCCEEDED";
			break;

		case ENET_PEER_STATE_CONNECTED:
			return "ENET_PEER_STATE_CONNECTED";
			break;

		case ENET_PEER_STATE_DISCONNECT_LATER:
			return "ENET_PEER_STATE_DISCONNECT_LATER";
			break;

		case ENET_PEER_STATE_DISCONNECTING:
			return "ENET_PEER_STATE_DISCONNECTING";
			break;

		case ENET_PEER_STATE_ACKNOWLEDGING_DISCONNECT:
			return "ENET_PEER_STATE_ACKNOWLEDGING_DISCONNECT";
			break;

		case ENET_PEER_STATE_ZOMBIE:
			return "ENET_PEER_STATE_ZOMBIE";
			break;

		default:
			return "ENET Unknown peer state.";
	}
}

//--------------------------------------------------------------------------------------------------------------------
//
// Turn an enet event type into a string
std::string getEventType(int whichEventType)
//--------------------------------------------------------------------------------------------------------------------
{
	switch (whichEventType)
	{
		case ENET_EVENT_TYPE_NONE:
			return "ENET_EVENT_TYPE_NONE";
			break;

		case ENET_EVENT_TYPE_CONNECT:
			return "ENET_EVENT_TYPE_CONNECT";
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			return "ENET_EVENT_TYPE_DISCONNECT";
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			return "ENET_EVENT_TYPE_RECEIVE";
			break;

		default:
			return "Unknown ENET event type";
	}
}

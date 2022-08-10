#include "com_util.h"
#include <vector>
#include <com_globals.h>
#include <regex>

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

	printf ("Get host from address [ %u ]\n", whichAddress.host);

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

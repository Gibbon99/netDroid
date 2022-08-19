#include "com_message.h"
#include "com_util.h"

#define IS_CLIENT 1

void droidMessage::AddRef()
{

}

void droidMessage::ReleaseRef()
{

}

//----------------------------------------------------------------------------------------------------------------------
//
// Set the filename to be used for writing to file
droidMessage::droidMessage(std::string fileName)
//----------------------------------------------------------------------------------------------------------------------
{
	logFileName = std::move (fileName);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Flush the ofstream and then close it
droidMessage::~droidMessage()
//----------------------------------------------------------------------------------------------------------------------
{
	if (fileLoggingOn)
	{
		flush (outFile);
		outFile.close ();
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Send a message to a target
void droidMessage::message(int target, const std::string& messageText)
//----------------------------------------------------------------------------------------------------------------------
{
#ifdef Release
	if (target & MESSAGE_TARGET_DEBUG)
		return;
#else
	if (target & MESSAGE_TARGET_DEBUG)
		std::cout << "DEBUG : " << messageText << std::endl;
#endif

	if (target & MESSAGE_TARGET_STD_OUT)
	{
		std::cout << messageText << std::endl;
	}

	if (target & MESSAGE_TARGET_LOGFILE)
	{
		write(sys_getString("%s", messageText.c_str()));
	}

	if (target & MESSAGE_TARGET_CONSOLE)
	{
		consoleLines.push_back(messageText);
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return all the messages meant for the console - return in vector of std::strings
std::vector<std::string> droidMessage::getStrings()
//----------------------------------------------------------------------------------------------------------------------
{
	return consoleLines;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Clear the vector of console strings - have them all in the console now
void droidMessage::clearStrings()
//----------------------------------------------------------------------------------------------------------------------
{
	consoleLines.clear();
}

//----------------------------------------------------------------------------------------------------------------------
//
// Open the file on the disk
bool droidMessage::open()
//----------------------------------------------------------------------------------------------------------------------
{
	outFile.open (logFileName, std::ios::binary);
	if (!outFile)
	{
		lastError = sys_getString("Unable to open file [ %s ]", logFileName.c_str());
		return false;
	}

	fileLoggingOn = true;
	outFile << "Logfile [ " << logFileName << " ] opened [ " << sys_getDate () << " ]" << std::endl;
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Write a string to the open logfile
void droidMessage::write(const std::string &logText)
//----------------------------------------------------------------------------------------------------------------------
{
	if (!fileLoggingOn)
		open();

	if (fileLoggingOn)
		outFile << "[ " <<  sys_getTime () << " ] " << logText << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Check if file logging is available
bool droidMessage::enabled() const
//----------------------------------------------------------------------------------------------------------------------
{
	return fileLoggingOn;
}

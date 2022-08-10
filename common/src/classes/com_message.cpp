#include "com_message.h"
#include "com_util.h"

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
	_consoleLine    tempConsoleLine;

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
		tempConsoleLine.red = 255;
		tempConsoleLine.green = 255;
		tempConsoleLine.blue = 255;
		tempConsoleLine.consoleLine = messageText;
		consoleText.push_back (tempConsoleLine);
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the number of lines currently in the console vector
int droidMessage::getNumberOfLines()
//----------------------------------------------------------------------------------------------------------------------
{
	return static_cast<int>(consoleText.size());
}

//----------------------------------------------------------------------------------------------------------------------
//
// Get the text from the passed in index for the console
std::string droidMessage::getConsoleLine(int lineIndex)
//----------------------------------------------------------------------------------------------------------------------
{
	if (lineIndex > static_cast<int>(consoleText.size() - 1))
	{
		lastError = sys_getString("Index [ %i ] out of bounds [ %i ].", lineIndex, consoleText.size());
		return lastError;
	}

	return consoleText[lineIndex].consoleLine;
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

#include <regex>
#include "com_console.h"
#include "../../../client/hdr/main.h"

//--------------------------------------------------------------------------------------------------------------------
//
// Constructor
droidConsole::droidConsole (float newDefaultPosX, int red, int green, int blue, int alpha)
//--------------------------------------------------------------------------------------------------------------------
{
	defaultRed       = red;
	defaultGreen     = green;
	defaultBlue      = blue;
	defaultAlpha     = alpha;
	defaultPosX      = newDefaultPosX;
	scrollbackOffset = 0;
	currentLineIndex = 0;
}

//-----------------------------------------------------------------------------------------------------------------------
//
// Deconstruct
droidConsole::~droidConsole ()
//-----------------------------------------------------------------------------------------------------------------------
{
//	for (auto variableItr: consoleVariables)
	{

	}
}

//-----------------------------------------------------------------------------------------------------------------------
//
// Return a line of text - return empty string if invalid
std::string droidConsole::getLine ()
//-----------------------------------------------------------------------------------------------------------------------
{
	if (consoleText.empty())
		return "";

	if (currentLineIndex > 0)
		currentLineIndex--;

	return consoleText[currentLineIndex].lineText;
}

//--------------------------------------------------------------------------------------------------------------------
//
// Prepare console
void droidConsole::prepare (float newPosX, float newPosY)
//--------------------------------------------------------------------------------------------------------------------
{
//	consoleItr       = consoleText.rbegin () + scrollbackOffset;
	posX             = newPosX;
	posY             = newPosY;
	currentLineIndex = consoleText.size() - scrollbackOffset;
}

//--------------------------------------------------------------------------------------------------------------------
//
// Return x Position
float droidConsole::getDefaultPosX () const
//--------------------------------------------------------------------------------------------------------------------
{
	return defaultPosX;
}

//--------------------------------------------------------------------------------------------------------------------
//
// Add line to console - loop until drawing thread is finished
void droidConsole::addLine (const _consoleLine &newTempLine)
//--------------------------------------------------------------------------------------------------------------------
{
	if (consoleText.size () < CONSOLE_MEM_SIZE)
		consoleText.push_back (newTempLine);
}

//--------------------------------------------------------------------------------------------------------------------
//
// Add new colored line of text
void droidConsole::add (float linePosX, int red, int green, int blue, int alpha, const std::string &newLine)
//--------------------------------------------------------------------------------------------------------------------
{
	_consoleLine tempLine;

	tempLine.lineText = newLine;
	tempLine.posX     = linePosX;
	tempLine.red      = red;
	tempLine.green    = green;
	tempLine.blue     = blue;
	tempLine.alpha    = alpha;

	addLine (tempLine);
}

//--------------------------------------------------------------------------------------------------------------------
//
// Add line of text to console
void droidConsole::add (const std::string &newLine)
//--------------------------------------------------------------------------------------------------------------------
{
	_consoleLine tempLine;

	tempLine.lineText = newLine;
	tempLine.posX     = defaultPosX;
	tempLine.red      = defaultRed;
	tempLine.green    = defaultGreen;
	tempLine.blue     = defaultBlue;
	tempLine.alpha    = defaultAlpha;

	addLine (tempLine);
}

//--------------------------------------------------------------------------------------------------------------------
//
// Break up entry line into tokens
std::vector<std::string> droidConsole::tokeniseEntryLine (std::string entryLine)
//--------------------------------------------------------------------------------------------------------------------
{
	//
	// https://stackoverflow.com/a/27468529/4621054
	std::regex reg ("\\s+");

	std::sregex_token_iterator iter (entryLine.begin (), entryLine.end (), reg, -1);
	std::sregex_token_iterator end;

	std::vector<std::string> enteredCommand (iter, end);

	return enteredCommand;
}
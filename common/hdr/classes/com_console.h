#pragma once

#include <string>
#include <vector>
#include <map>

#define CONSOLE_MEM_SIZE    1000    // Max number of lines in vector#include <string>


typedef void (*funcPtr) ();      // Pointer to function with no args and returns void
typedef void (*funcPtr_int) (int);

//
// This runs a script command
struct _consoleCommand
{
	std::string commandName;
	std::string commandHelp;
	funcPtr     commandPtr;
};

//
// This runs an internal function via function pointer
struct _consoleFunction
{
	void        *functionPtr;
	std::string functionHelp;
};

struct _variables
{
	std::string varName;
	int         varType;
	int         *varPtrInt; //    = new int;
	bool        *varPtrBool; //   = new bool;
	float       *varPtrFloat; //  = new float;
	std::string *varPtrString; // = new std::string;
};

struct _consoleLine
{
	float       posX;
	int         red;
	int         green;
	int         blue;
	int         alpha;
	std::string lineText;
};

class droidConsole
{
public:

	droidConsole (float newDefaultPosX, int red, int green, int blue, int alpha);

	~droidConsole ();

	void addLine (const _consoleLine &newTempLine);

	void add (float linePosX, int red, int green, int blue, int alpha, const std::string &newLine);

	void add (const std::string &newLine);

	std::vector<std::string> tokeniseEntryLine (std::string entryLine);

	void prepare (float newPosX, float newPosY);

	float getDefaultPosX () const;

//	std::vector<_consoleLine>::reverse_iterator consoleItr{};

	std::string getLine ();

private:

	int                       defaultRed{};
	int                       defaultGreen{};
	int                       defaultBlue{};
	int                       defaultAlpha{};
	int                       userBufferIndex{};
	int                       scrollbackOffset{};
	int                       currentLineIndex{};
	float                     defaultPosX{};
	float                     posX{};
	float                     posY{};
	std::vector<_consoleLine> consoleText{};
};

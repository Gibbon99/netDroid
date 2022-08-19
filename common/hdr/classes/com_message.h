#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

enum MESSAGE_TARGETS
{
	MESSAGE_TARGET_STD_OUT = 1 << 0, // 1
	MESSAGE_TARGET_LOGFILE = 1 << 2,
	MESSAGE_TARGET_CONSOLE = 1 << 4,
	MESSAGE_TARGET_DEBUG   = 4 << 8
};

class droidMessage
{
public:
	void AddRef ();

	void ReleaseRef ();

	void message (int target, const std::string &messageText);

	explicit droidMessage (std::string fileName);

	std::string getConsoleLine (int lineIndex);

	int getNumberOfLines ();

	~droidMessage ();

	bool open ();

	void write (const std::string &logText);

	bool enabled () const;

	std::vector<std::string> getStrings();

	void clearStrings();

private:
	std::string              lastError{};
	std::string              logFileName{};
	bool                     fileLoggingOn = false;
	std::ofstream            outFile{};
	std::vector<std::string> consoleLines{};
};

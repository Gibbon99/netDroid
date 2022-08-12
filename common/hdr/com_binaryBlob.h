#pragma once

#include <string>
#include <vector>

class droidBinaryBlob
{
public:
	bool loadFromDisk (const std::string &newFileName);

	std::vector<int8_t> getBlob ();

	int getBlobSize ();

	~droidBinaryBlob ();

	std::string getLastError ();

private:
	std::vector<int8_t> buffer{};
	int                 bufferSize{};
	std::string         lastError{};
};

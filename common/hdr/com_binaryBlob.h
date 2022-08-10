#pragma once

#include <string>

class droidBinaryBlob
{
public:
	bool loadFromDisk (const std::string &newFileName);

	std::vector<int8_t> getBlob ();

	int getBlobSize ();

	~droidBinaryBlob ();

private:
	std::vector<int8_t> buffer{};
	int              bufferSize{};
};

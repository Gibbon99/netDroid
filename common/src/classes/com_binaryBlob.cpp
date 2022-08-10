#include <fstream>
#include <vector>
#include <iterator>
#include "com_binaryBlob.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Free any memory used by binary blob
droidBinaryBlob::~droidBinaryBlob()
//----------------------------------------------------------------------------------------------------------------------
{
    buffer.clear();
}

//----------------------------------------------------------------------------------------------------------------------
//
// Load a binary file from disk into memory
bool droidBinaryBlob::loadFromDisk (const std::string &newFileName)
//----------------------------------------------------------------------------------------------------------------------
{
    // open the file:
    std::ifstream file(newFileName, std::ios::binary);

    // Stop eating new lines in binary mode
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    buffer.clear();
    buffer.reserve(fileSize);

    // read the data:
    buffer.insert(buffer.begin(),
               std::istream_iterator<int8_t>(file),
               std::istream_iterator<int8_t>());

    bufferSize = buffer.size();

    return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return a pointer to the blob held in memory
std::vector<int8_t> droidBinaryBlob::getBlob()
//----------------------------------------------------------------------------------------------------------------------
{
	return buffer;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the size of the buffer in memory
int droidBinaryBlob::getBlobSize()
//----------------------------------------------------------------------------------------------------------------------
{
	return bufferSize;
}

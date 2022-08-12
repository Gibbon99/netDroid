#include <fstream>
#include <vector>
#include <iterator>
#include "com_binaryBlob.h"
#include "com_util.h"

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
// Return the last error string
std::string droidBinaryBlob::getLastError()
//----------------------------------------------------------------------------------------------------------------------
{
    return lastError;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Load a binary file from disk into memory
bool droidBinaryBlob::loadFromDisk (const std::string &newFileName)
//----------------------------------------------------------------------------------------------------------------------
{
    // open the file:
    std::ifstream file(newFileName, std::ios::binary);
    //
    // Check it worked
    if (file.fail())
    {
        lastError = sys_getString("Unable to open file [ %s ].", newFileName.c_str());
        return false;
    }
    //
    // Stop eating new lines in binary mode
    file.unsetf(std::ios::skipws);
    //
    // get its size:
    std::streampos fileSize;
    //
    // Seek to the end
    file.seekg(0, std::ios::end);
    if (file.fail())
    {
        lastError = sys_getString("Unable to seek to end of file [ %s ].", newFileName.c_str());
        return false;
    }
    fileSize = file.tellg();
    if (fileSize < 0)
    {
        lastError = sys_getString("Error getting position in file stream [ %s ].", newFileName.c_str());
        return false;
    }

    file.seekg(0, std::ios::beg);
    if (file.fail())
    {
        lastError = sys_getString("Unable to seek to start of file [ %s ].", newFileName.c_str());
        return false;
    }

    // reserve capacity
    buffer.clear();
    buffer.reserve(fileSize);

    // read the data:
    buffer.insert(buffer.begin(),
               std::istream_iterator<int8_t>(file),
               std::istream_iterator<int8_t>());

    bufferSize = buffer.size();
    if (bufferSize != fileSize)
    {
        lastError = sys_getString("Error occurred reading file into memory buffer [ %s ].", newFileName.c_str());
        return false;
    }

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

#include <GL/glew.h>
#include "../../hdr/openGL/c_glWrap.h"

std::map <int, std::string> glBuffersMap;

//----------------------------------------------------------------------------------------------------------------------
//
// Wrap the call to generate a buffer ID so we can print out some text with it
uint wrapglGenBuffers(int numBuffers, const std::string& bufferText)
//----------------------------------------------------------------------------------------------------------------------
{
	GLuint			bufferID;

	glGenBuffers(numBuffers, &bufferID);
	//
	// Add the generated buffer ID and the text to a STD::MAP
	glBuffersMap.insert (std::make_pair( bufferID, bufferText));

	return bufferID;
}
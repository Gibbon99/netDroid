#include "../../hdr/system/c_graphics.h"
#include "../../hdr/openGL/c_glWrap.h"

std::map<std::string, _droidShaders> droidShaders{};

bool init2DQuad (std::string newShaderName)
{

	_droidShaders tempShader;

	std::string vertShader = "#version 330\n"
	                         "\n"
	                         "//\n"
	                         "// Draws a texture mapped 2d quad\n"
	                         "//\n"
	                         "\n"
	                         "uniform vec2       inScreenSize;\n"
	                         "\n"
	                         "layout (location = 0) in vec3     inPosition;\n"
	                         "layout (location = 1) in vec2     inTextureCoords;\n"
	                         "\n"
	                         "out vec2 texCoord0;\n"
	                         "\n"
	                         "void main(void)\n"
	                         "{\n"
	                         "\ttexCoord0 = inTextureCoords;\n"
	                         "\n"
	                         "\tvec2 vertexPosition = inPosition.xy - inScreenSize.xy;\n"
	                         "\tvertexPosition /= inScreenSize.xy;\n"
	                         "\n"
	                         "\tgl_Position =  vec4(vertexPosition,0,1);\n"
	                         "}";

	std::string fragShader = "#version 330\n"
	                         "\n"
	                         "uniform sampler2D inTexture0;\n"
	                         "uniform float gamma;\n"
	                         "\n"
	                         "in vec2 texCoord0;\n"
	                         "\n"
	                         "layout(location = 0)    out     vec4        pixelColor;     // Relates to frameBuffer - writing to render buffer 0 - which is linked to the targetTexture as GL_COLOR_ATTACHMENT 0\n"
	                         "\n"
	                         "vec4 textureColor;\n"
	                         "\n"
	                         "void main()\n"
	                         "{\n"
	                         "\ttextureColor = texture2D(inTexture0, texCoord0.st);\n"
	                         "\t//\n"
	                         "\t// Apply Gamma setting to the texture colors\n"
	                         "\tvec4 finalGamma = vec4 (1.0 / gamma);\n"
	                         "\tfinalGamma.w = 1.0;\n"
	                         "\tpixelColor = pow (textureColor, finalGamma);\n"
	                         "}";

	if (!tempShader.create (vertShader, fragShader, newShaderName))
	{
		clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("%s", tempShader.getLastError ().c_str ()));
		return false;
	}

	droidShaders[newShaderName] = tempShader;

	clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("Shader [ %s ] linked and compiled ok.", newShaderName.c_str()));

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Convert a datapacket image to a surface ready for display on the client
void c_convertPacketToSurface (dataPacket newDataPacket)
//----------------------------------------------------------------------------------------------------------------------
{
	SDL_RWops *rw = SDL_RWFromMem (static_cast<void *>(&newDataPacket.binaryData[0]), newDataPacket.binarySize);
	if (nullptr == rw)
	{
		clientMessage.message (MESSAGE_TARGET_STD_OUT, sys_getString ("Couldn't create RWops [ %s ]", SDL_GetError ()));
		return;
	}

	auto tempSurface = IMG_LoadJPG_RW (rw);
	if (nullptr == tempSurface)
	{
		clientMessage.message (MESSAGE_TARGET_STD_OUT, sys_getString ("Unable to load image from memory ; [ %s ]", SDL_GetError ()));
		return;
	}

	clientMessage.message (MESSAGE_TARGET_STD_OUT, sys_getString ("Client: Loaded JPG from binaryData."));

//    testTexture.convertToTexture(clientWindow.getRenderer(),  tempSurface);

	clientMessage.message (MESSAGE_TARGET_DEBUG, sys_getString ("Created texture from surface W [ %i ] h [ %i ] format [ %i ] access [ %i ].", testTexture.getWidth (), testTexture.getHeight (), SDL_PIXELTYPE(testTexture.getFormat ()), testTexture.getAccess ()));
}

//-----------------------------------------------------------------------------
//
// Return the Shader ID
GLuint gl_getShaderID (std::string shaderName)
//-----------------------------------------------------------------------------
{
	auto shaderIDIter = droidShaders.find (shaderName);

	if (shaderIDIter == droidShaders.end ())
	{
		clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("Unable to find shaderID by name [ %s ]", shaderName.c_str ()));
		return 0;
	}

	if (!shaderIDIter->second.isCompiled ())
	{
		clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("Shader [ %s ] is not ready for use.", shaderName.c_str ()));
		return 0;
	}

	return droidShaders[shaderName].getProgramID ();
}

//-----------------------------------------------------------------------------
//
// Draw a 2D quad - uses ortho matrix to draw - screen pixel coordinates
void c_draw2DQuad (glm::vec2 position, glm::vec2 quadSize, const std::string &whichShader, GLuint whichTexture, float textureCoords[])
//-----------------------------------------------------------------------------
{
	glm::vec3     quadVerts[4];
	static GLuint vao{};
	static GLuint buffers[2];
	static bool   initDone{false};

	quadVerts[0].x = position.x;
	quadVerts[0].y = position.y;
	quadVerts[0].z = 0.0f;

	quadVerts[1].x = position.x;
	quadVerts[1].y = position.y + quadSize.y;
	quadVerts[1].z = 0.0f;

	quadVerts[2].x = position.x + quadSize.x;
	quadVerts[2].y = position.y + quadSize.y;
	quadVerts[2].z = 0.0f;

	quadVerts[3].x = position.x + quadSize.x;
	quadVerts[3].y = position.y;
	quadVerts[3].z = 0.0f;

	quadVerts[0].x = position.x - (quadSize.x / 2);
	quadVerts[0].y = position.y - (quadSize.y / 2);
	quadVerts[0].z = 0.0f;

	quadVerts[1].x = position.x - (quadSize.x / 2);
	quadVerts[1].y = position.y + (quadSize.y / 2);
	quadVerts[1].z = 0.0f;

	quadVerts[2].x = position.x + (quadSize.y / 2);
	quadVerts[2].y = position.y + (quadSize.y / 2);
	quadVerts[2].z = 0.0f;

	quadVerts[3].x = position.x + (quadSize.y / 2);
	quadVerts[3].y = position.y - (quadSize.y / 2);
	quadVerts[3].z = 0.0f;

	if (!initDone)
	{
		// create the VAO
		GL_ASSERT (glGenVertexArrays (1, &vao));
		GL_CHECK (glBindVertexArray (vao));
		//
		// Create buffers for the vertex data
		buffers[0] = wrapglGenBuffers (1, __func__);
		buffers[1] = wrapglGenBuffers (1, __func__);

		GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));
		//
		// Vertex coordinates buffer
		GL_ASSERT (glBindBuffer (GL_ARRAY_BUFFER, buffers[0]));
		GL_CHECK (glBufferData (GL_ARRAY_BUFFER, sizeof (quadVerts), quadVerts, GL_DYNAMIC_DRAW));
		GL_CHECK (glEnableVertexAttribArray (droidShaders[whichShader].getLocation ("inPosition")));
		GL_CHECK (glVertexAttribPointer (droidShaders[whichShader].getLocation ("inPosition"), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET (0)));
		//
		// Texture coordinates buffer
		GL_ASSERT (glBindBuffer (GL_ARRAY_BUFFER, buffers[1]));
		GL_CHECK (glBufferData (GL_ARRAY_BUFFER, sizeof (float) * 8, textureCoords, GL_DYNAMIC_DRAW));
		GL_CHECK (glEnableVertexAttribArray (droidShaders[whichShader].getLocation ("inTextureCoords")));
		GL_CHECK (glVertexAttribPointer (droidShaders[whichShader].getLocation ("inTextureCoords"), 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET (0)));

		initDone = false;
	}

	GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));
	//
	// Bind texture if it's not already bound as current texture
	GL_CHECK (glActiveTexture (GL_TEXTURE0));
	GL_CHECK (glBindTexture (GL_TEXTURE_2D, whichTexture));

	GL_CHECK (glUniform1i (droidShaders[whichShader].getLocation ("inTexture0"), 0));

	GL_CHECK (glBindVertexArray (vao));
	//
	// Enable attribute to hold vertex information
	GL_CHECK (glEnableVertexAttribArray (droidShaders[whichShader].getLocation ("inPosition")));
	GL_CHECK (glEnableVertexAttribArray (droidShaders[whichShader].getLocation ("inTextureCoords")));

	auto MVP = glm::ortho (0.0f, static_cast<float>(clientWindow.getWidth ()), 0.0f, static_cast<float>(clientWindow.getHeight ())) * (glm::mat4 (1.0f) * glm::mat4 (1.0f));

	GL_CHECK (glUniformMatrix4fv (droidShaders[whichShader].getLocation ("MVP_Matrix"), 1, false, glm::value_ptr (MVP)));

	GL_CHECK (glDrawArrays (GL_TRIANGLE_FAN, 0, 4));

	glDeleteBuffers (2, buffers);
	glDeleteVertexArrays (1, &vao);
}

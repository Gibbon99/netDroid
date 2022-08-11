#include "../../hdr/system/c_graphics.h"
#include "../../hdr/openGL/c_glWrap.h"
#include "../../hdr/system/c_events.h"

std::map<std::string, _droidShaders> droidShaders{};

bool init2DQuad (std::string newShaderName)
{

	_droidShaders tempShader;

	std::string vertShader = "#version 330\n"
	                         "\n"
	                         "uniform vec2 i_ScreenSize;\n"
	                         "uniform sampler2D i_Texture0;\n"
	                         "\n"
	                         "in vec3 i_Position;\n"
	                         "in vec2 i_TextureCoords;\n"
	                         "\n"
	                         "out vec2 texCoord0;\n"
	                         "\n"
	                         "void main(void)\n"
	                         "{\n"
	                         "\ttexCoord0 = i_TextureCoords;\n"
	                         "\n"
	                         "\tvec2 vertexPosition = i_Position.xy - i_ScreenSize.xy;\n"
	                         "\tvertexPosition /= i_ScreenSize.xy;\n"
	                         "\tgl_Position =  vec4(vertexPosition,0,1);\n"
	                         "}";

	std::string fragShader = "#version 330\n"
	                         "\n"
	                         "uniform sampler2D   i_Texture0;\n"
	                         "\n"
	                         "in      vec2        texCoord0;\n"
	                         "out     vec4        outColor;\n"
	                         "\n"
	                         "void main()\n"
	                         "{\n"
	                         "    vec4 texColor = texture2D(i_Texture0, texCoord0.st);\n"
	                         "    outColor = vec4(texColor.rgb, 1.0f);\n"
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
// Convert a datapacket image to an SDL_surface ready for display on the client
// Generate an event to be run on the main thread to create the OpenGL texture ( needs main thread context )
void c_convertPacketToSurface (dataPacket newDataPacket)
//----------------------------------------------------------------------------------------------------------------------
{
	droidTexture    tempTexture;

	clientTextures.insert(std::pair<std::string, droidTexture>(newDataPacket.testString, tempTexture));

	SDL_RWops *rw = SDL_RWFromMem (static_cast<void *>(&newDataPacket.binaryData[0]), newDataPacket.binarySize);
	if (nullptr == rw)
	{
		clientMessage.message (MESSAGE_TARGET_STD_OUT, sys_getString ("Couldn't create RWops [ %s ]", SDL_GetError ()));
		return;
	}

	clientTextures[newDataPacket.testString].setSurface(IMG_LoadJPG_RW (rw));
	/*
	if (nullptr == tempTexture.getSurface())
	{
		clientMessage.message (MESSAGE_TARGET_STD_OUT, sys_getString ("Unable to load image from memory ; [ %s ]", SDL_GetError ()));
		return;
	}
*/


	evt_sendEvent ( GAME_LOOP_EVENT, USER_EVENT_TEXTURE_UPLOAD, 0, 0, 0, glm::vec2{}, glm::vec2{}, newDataPacket.testString);
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
		GL_CHECK (glEnableVertexAttribArray (droidShaders[whichShader].getLocation ("i_Position")));
		GL_CHECK (glVertexAttribPointer (droidShaders[whichShader].getLocation ("i_Position"), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET (0)));
		//
		// Texture coordinates buffer
		GL_ASSERT (glBindBuffer (GL_ARRAY_BUFFER, buffers[1]));
		GL_CHECK (glBufferData (GL_ARRAY_BUFFER, sizeof (float) * 8, textureCoords, GL_DYNAMIC_DRAW));
		GL_CHECK (glEnableVertexAttribArray (droidShaders[whichShader].getLocation ("i_TextureCoords")));
		GL_CHECK (glVertexAttribPointer (droidShaders[whichShader].getLocation ("i_TextureCoords"), 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET (0)));

		initDone = false;
	}

	GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));
	//
	// Bind textureID if it's not already bound as current textureID
	GL_CHECK (glActiveTexture (GL_TEXTURE0));
	GL_CHECK (glBindTexture (GL_TEXTURE_2D, whichTexture));

	GL_CHECK (glUniform1i (droidShaders[whichShader].getLocation ("i_Texture0"), 0));

	GL_CHECK (glBindVertexArray (vao));
	//
	// Enable attribute to hold vertex information
	GL_CHECK (glEnableVertexAttribArray (droidShaders[whichShader].getLocation ("i_Position")));
	GL_CHECK (glEnableVertexAttribArray (droidShaders[whichShader].getLocation ("i_TextureCoords")));

//	auto MVP = glm::ortho (0.0f, static_cast<float>(clientWindow.getWidth ()), 0.0f, static_cast<float>(clientWindow.getHeight ())) * (glm::mat4 (1.0f) * glm::mat4 (1.0f));
//	GL_CHECK (glUniformMatrix4fv (droidShaders[whichShader].getLocation ("MVP_Matrix"), 1, false, glm::value_ptr (MVP)));

	GL_CHECK ( glUniform2f (droidShaders[whichShader].getLocation("i_ScreenSize"), (float)clientWindow.getWidth () / 4, (float)clientWindow.getHeight () / 4));

	GL_CHECK (glDrawArrays (GL_TRIANGLE_FAN, 0, 4));

	glDeleteBuffers (2, buffers);
	glDeleteVertexArrays (1, &vao);
}

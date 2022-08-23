#include "../../hdr/system/c_graphics.h"
#include "../../hdr/openGL/c_glWrap.h"
#include "../../hdr/system/c_events.h"

std::map<std::string, _droidShaders> droidShaders{};

bool init2DQuad (std::string newShaderName)
{

	_droidShaders tempShader;

	std::string vertShader = "#version 330\n"
	                         "\n"
	                         "uniform mat4 MVP;\n"
	                         "uniform sampler2D i_Texture0;\n"
	                         "\n"
	                         "in vec3 i_Position;\n"
	                         "in vec2 i_TextureCoords;\n"
	                         "\n"
	                         "out vec2 TextureCoords;\n"
	                         "\n"
	                         "void main()\n"
	                         "{\n"
	                         "  TextureCoords = i_TextureCoords;\n"
	                         "  gl_Position =  MVP * vec4(i_Position, 1.0);\n"
	                         "}";

	std::string fragShader = "#version 330\n"
	                         "\n"
	                         "uniform sampler2D   u_Texture0;\n"
	                         "\n"
	                         "in      vec2        TextureCoords;\n"
	                         "\n"
	                         "out     vec4        pixelColor;\n"
	                         "\n"
	                         "void main()\n"
	                         "{\n"
	                         "    vec4 texColor = texture2D(u_Texture0, TextureCoords.xy);\n"
	                         "    pixelColor = vec4(texColor.rgb, 1.0f);\n"
	                         "}";

	if (!tempShader.create (vertShader, fragShader, newShaderName))
	{
		clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("%s", tempShader.getLastError ().c_str ()));
		return false;
	}

	droidShaders[newShaderName] = tempShader;

	clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("Shader [ %s ] linked and compiled ok.", newShaderName.c_str ()));

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Convert a datapacket image to an SDL_surface ready for display on the client
// Generate an event to be run on the main thread to create the OpenGL texture ( needs main thread context )
void c_convertPacketToSurface (dataPacket newDataPacket)
//----------------------------------------------------------------------------------------------------------------------
{
	droidTexture tempTexture;

	clientTextures.insert (std::pair<std::string, droidTexture> (newDataPacket.testString, tempTexture));
	clientTextures[newDataPacket.testString].setName (newDataPacket.testString);

	SDL_RWops *rw = SDL_RWFromMem (static_cast<void *>(&newDataPacket.binaryData[0]), newDataPacket.binarySize);
	if (nullptr == rw)
	{
		clientTextures[newDataPacket.testString].setSurfaceState (NOT_LOADED);
		clientTextures[newDataPacket.testString].setTextureState (NOT_LOADED);
		clientMessage.message (MESSAGE_TARGET_STD_OUT, sys_getString ("Couldn't create RWops [ %s ]", SDL_GetError ()));
		return;
	}

	clientTextures[newDataPacket.testString].setSurface (IMG_Load_RW (rw, 0), true);
	if (nullptr == clientTextures[newDataPacket.testString].getSurface ())
	{
		clientTextures[newDataPacket.testString].setSurfaceState (NOT_LOADED);
		clientTextures[newDataPacket.testString].setTextureState (NOT_LOADED);
		clientMessage.message (MESSAGE_TARGET_STD_OUT, sys_getString ("Unable to load image [ %s ] from memory ; [ %s ]", newDataPacket.testString.c_str (), SDL_GetError ()));
		return;
	}

	c_addEventToQueue (EventType::EVENT_GAME_LOOP, EventAction::ACTION_UPLOAD_TEXTURE, 0, 0, 0, glm::vec2{}, glm::vec2{}, newDataPacket.testString);
}

//-----------------------------------------------------------------------------
//
// Return the Shader ID
GLuint c_getShaderID (const std::string &shaderName)
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
// Draw a 2d textured quad
void c_draw2DQuad_ext (glm::vec2 position, glm::vec2 quadSize, [[maybe_unused]]const std::string &whichShaer, [[maybe_unused]]GLuint whichTexture)
//-----------------------------------------------------------------------------
{
	GLfloat Vertices[] = {position.x, position.y, 0, position.x + quadSize.x, position.y, 0, position.x + quadSize.x, position.y + quadSize.y, 0, position.x, position.y + quadSize.y, 0};

	GLfloat TexCoord[] = {0, 0, 1, 0, 1, 1, 0, 1,};

	GLubyte indices[] = {0, 1, 2, // first triangle (bottom left - top left - top right)
	                     0, 2, 3}; // second triangle (bottom left - top right - bottom right)

	glEnableClientState (GL_VERTEX_ARRAY);
	glVertexPointer (3, GL_FLOAT, 0, Vertices);

	glEnableClientState (GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer (2, GL_FLOAT, 0, TexCoord);

	glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
	glDisableClientState (GL_VERTEX_ARRAY);
}

//-----------------------------------------------------------------------------
//
// Draw a 2D quad - uses ortho matrix to draw - screen pixel coordinates
bool c_draw2DQuad (glm::vec2 position, glm::vec2 quadSize, const std::string &whichShader, GLuint whichTexture, float textureCoords[])
//-----------------------------------------------------------------------------
{
	glm::vec3     quadVerts[4];
	static GLuint vao{};
	static GLuint buffers[2];
	static bool   initDone{false};
	GLuint        useShaderID{};

	GLubyte indices[] = {0, 1, 2,      // first triangle (bottom left - top left - top right)
	                     0, 2, 3};     // second triangle (bottom left - top right - bottom right)

	quadVerts[0].x = position.x;
	quadVerts[0].y = position.y;
	quadVerts[0].z = 0.0f;
	quadVerts[1].x = position.x + quadSize.x;
	quadVerts[1].y = position.y;
	quadVerts[1].z = 0.0f;
	quadVerts[2].x = position.x + quadSize.x;
	quadVerts[2].y = position.y + quadSize.y;
	quadVerts[2].z = 0.0f;
	quadVerts[3].x = position.x;
	quadVerts[3].y = position.y + quadSize.y;
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

		useShaderID = c_getShaderID (whichShader);
		if (0 == useShaderID)
		{
			clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("Unable to get shader ID for [ %s ]", whichShader.c_str ()));
			return false;
		}
		GL_CHECK (glUseProgram (useShaderID));
		//
		// Vertex coordinates buffer
		GL_ASSERT (glBindBuffer (GL_ARRAY_BUFFER, buffers[0]));
		GL_CHECK (glBufferData (GL_ARRAY_BUFFER, sizeof (quadVerts), &quadVerts[0], GL_DYNAMIC_DRAW));
		GL_CHECK (glEnableVertexAttribArray (droidShaders[whichShader].getLocation ("i_Position")));
		GL_CHECK (glVertexAttribPointer (droidShaders[whichShader].getLocation ("i_Position"), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET (0)));
		//
		// Texture coordinates buffer
		GL_ASSERT (glBindBuffer (GL_ARRAY_BUFFER, buffers[1]));
		GL_CHECK (glBufferData (GL_ARRAY_BUFFER, sizeof (float) * 8, textureCoords, GL_DYNAMIC_DRAW));
		GL_CHECK (glEnableVertexAttribArray (droidShaders[whichShader].getLocation ("i_TextureCoords")));
		GL_CHECK (glVertexAttribPointer (droidShaders[whichShader].getLocation ("i_TextureCoords"), 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET (0)));

		initDone = true;
	}

	useShaderID = c_getShaderID (whichShader);
	if (0 == useShaderID)
	{
		clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("Unable to get shader ID for [ %s ]", whichShader.c_str ()));
		return false;
	}
	GL_CHECK (glUseProgram (useShaderID));
	GL_CHECK (glBindVertexArray (vao));
	//
	// Bind textureID if it's not already bound as current textureID
	GL_CHECK (glActiveTexture (GL_TEXTURE0));
	GL_CHECK (glBindTexture (GL_TEXTURE_2D, whichTexture));
	GL_CHECK (glUniform1i (droidShaders[whichShader].getLocation ("u_Texture0"), 0));

	auto projectionMatrix = glm::ortho (0.0f, static_cast<float>(clientWindow.getWidth ()), 0.0f, static_cast<float>(clientWindow.getHeight ()));
	auto MVP              = projectionMatrix * glm::mat4 (1.0f) * glm::mat4 (1.0f);
	GL_CHECK (glUniformMatrix4fv (droidShaders[whichShader].getLocation ("MVP"), 1, false, glm::value_ptr (MVP)));

	GL_CHECK (glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices));

//	glDeleteBuffers (2, buffers);
//	glDeleteVertexArrays (1, &vao);

	return true;
}

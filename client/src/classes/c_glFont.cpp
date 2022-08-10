#include "../../hdr/classes/c_glFont.h"
#include "com_util.h"

typedef struct
{
	float x, y, z;    // position
	float s, t;       // texture
	float r, g, b, a; // color
} vertex_t;

std::string codePoints = " ~!@#$%^&*()_+1234567890-=qwertyuiop[]\\asdfghjkl;'zxcvbnm,./QWERTYUIOP{}|ASDFGHJKL:ZXCVBNM<>?\"";

//----------------------------------------------------------------------------------------------------------------------
//
// Remove the font
droidGLFont::~droidGLFont ()
//----------------------------------------------------------------------------------------------------------------------
{
	texture_font_delete (font);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the last error message
std::string droidGLFont::returnLastError ()
//----------------------------------------------------------------------------------------------------------------------
{
	return lastError;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the height of the font
float droidGLFont::getHeight ()
//----------------------------------------------------------------------------------------------------------------------
{
	return height;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Create a texture atlas from passed in fontName and size
bool droidGLFont::init (std::string fontName, float fontSize, glm::vec2 textureSize)
//----------------------------------------------------------------------------------------------------------------------
{
	GLuint vs, fs;
	GLint  isCompiled = GL_FALSE;
	size_t missedGlyphs{};

	atlas = texture_atlas_new (textureSize.x, textureSize.y, 1);

	//	texture_font_new_from_memory ()     TODO - Get font file from server - load from memory

	font   = texture_font_new_from_file (atlas, fontSize, fontName.c_str ());
	if (nullptr == font)
	{
		lastError = sys_getString ("Unable to create texture font. Check fileName [ %s ]", fontName.c_str ());
		return false;
	}
	height = font->height;

	missedGlyphs = texture_font_load_glyphs (font, codePoints.c_str ());
	if (missedGlyphs > 0)
	{
		lastError = sys_getString ("Unable to render all glyphs. Missing [ %i ] from texture atlas. Texture size is too small.", missedGlyphs);
		return false;
	}

	glGenTextures (1, &atlas->id);
	glBindTexture (GL_TEXTURE_2D, atlas->id);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RED, atlas->width, atlas->height, 0, GL_RED, GL_UNSIGNED_BYTE, atlas->data);

	vertex_t vertices[4] = {{0,             0,             0, 0, 1, 0, 0, 0, 1},
	                        {0,             textureSize.y, 0, 0, 0, 0, 0, 0, 1},
	                        {textureSize.x, textureSize.y, 0, 1, 0, 0, 0, 0, 1},
	                        {textureSize.x, 0,             0, 1, 1, 0, 0, 0, 1}};

	GLuint indices[6] = {0, 1, 2, 0, 2, 3};
	buffer = vertex_buffer_new ("vertex:3f,tex_coord:2f,color:4f");
	vertex_buffer_push_back (buffer, vertices, 4, indices, 6);

	shaderProgramID = glCreateProgram ();

	vs = glCreateShader (GL_VERTEX_SHADER);
	glShaderSource (vs, 1, reinterpret_cast<const GLchar *const *>(&vertShader), nullptr);
	glCompileShader (vs);

	isCompiled = GL_FALSE;
	glGetShaderiv (vs, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv (vs, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		char *error = (char *) malloc (maxLength);
		glGetShaderInfoLog (vs, maxLength, &maxLength, error);
		lastError = sys_getString ("Font vertex shader error : [ %s ]", error);
		free (error);
		return false;
	}
	glAttachShader (shaderProgramID, vs);
	//
	// Fragment shader
	//
	fs = glCreateShader (GL_FRAGMENT_SHADER);
	glShaderSource (fs, 1, reinterpret_cast<const GLchar *const *>(&fragShader), nullptr);
	glCompileShader (fs);

	isCompiled = GL_FALSE;
	glGetShaderiv (fs, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv (fs, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		char *error = (char *) malloc (maxLength);
		glGetShaderInfoLog (fs, maxLength, &maxLength, error);
		lastError = sys_getString ("Font fragment shader error : [ %s ]", error);
		free (error);
		return false;
	}
	glAttachShader (shaderProgramID, fs);

	glLinkProgram (shaderProgramID);

	glDeleteShader (vs);
	glDeleteShader (fs);

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the width of a text string
float droidGLFont::getTextWidth (std::string newText)
//----------------------------------------------------------------------------------------------------------------------
{
	float kerning{0.0f};
	float newWidth{};

	for (auto charItr: newText)
	{
		texture_glyph_t *glyph = texture_font_get_glyph (font, &charItr);
		if (glyph != nullptr)
		{
			if (charItr != newText.front ())
			{
				kerning = texture_glyph_get_kerning (glyph, &charItr - 1);
			}

			newWidth += kerning;
			newWidth += glyph->advance_x;
		}
	}
	return newWidth;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Add some text - pass in desired print position
void droidGLFont::addText (glm::vec4 color, glm::vec2 position, const std::string &newText)
//----------------------------------------------------------------------------------------------------------------------
{
	GLuint indices[6] = {0, 1, 2, 0, 2, 3};
	float  kerning{0.0f};

	for (auto charItr: newText)
	{
		texture_glyph_t *glyph = texture_font_get_glyph (font, &charItr);
		if (glyph != nullptr)
		{
			if (charItr != newText.front ())
			{
				kerning = texture_glyph_get_kerning (glyph, &charItr - 1);
			}

			position.x += kerning;
			int x0 = (int) (position.x + glyph->offset_x);
			int y0 = (int) (position.y + glyph->offset_y);
			int x1 = (int) (x0 + glyph->width);
			int y1 = (int) (y0 - glyph->height);

			vertex_t vertices[4] = {{static_cast<float>(x0), static_cast<float>(y0), 0, glyph->s0, glyph->t0, color.r, color.g, color.b, color.a},
			                        {static_cast<float>(x0), static_cast<float>(y1), 0, glyph->s0, glyph->t1, color.r, color.g, color.b, color.a},
			                        {static_cast<float>(x1), static_cast<float>(y1), 0, glyph->s1, glyph->t1, color.r, color.g, color.b, color.a},
			                        {static_cast<float>(x1), static_cast<float>(y0), 0, glyph->s1, glyph->t0, color.r, color.g, color.b, color.a}};
			vertex_buffer_push_back (buffer, vertices, 4, indices, 6);
			position.x += glyph->advance_x;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Render all the vertices currently in the buffer in one batched call
void droidGLFont::render (glm::mat4 MVPMatrix)
//----------------------------------------------------------------------------------------------------------------------
{
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram (shaderProgramID);
	{
		glActiveTexture (GL_TEXTURE0);
		glBindTexture (GL_TEXTURE_2D, atlas->id);

		glUniform1i (glGetUniformLocation (shaderProgramID, "texture"), 0);
		glUniformMatrix4fv (glGetUniformLocation (shaderProgramID, "MVP"), 1, 0, glm::value_ptr (MVPMatrix));
		vertex_buffer_render (buffer, GL_TRIANGLES);
	}
	vertex_buffer_clear (buffer);
	glDisable (GL_BLEND);
}

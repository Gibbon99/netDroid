#pragma once

#include <string>
#include "freetype-gl.h"
#include "vertex-buffer.h"
#include "vec4.hpp"
#include "vec2.hpp"
#include "ext/matrix_float4x4.hpp"
#include "ext/matrix_clip_space.hpp"
#include "gtc/type_ptr.hpp"

class droidGLFont
{
public:

	~droidGLFont ();

	bool init (const std::string& fontName, float fontSize, glm::vec2 textureSize);

	std::string returnLastError ();

	void addText (glm::vec4 color, glm::vec2 position, const std::string &newText);

	void render (glm::mat4 MVPMatrix);

	float getHeight();

	float getTextWidth(std::string newText);

private:
	uint            shaderProgramID{};
	texture_atlas_t *atlas{};
	vertex_buffer_t *buffer{};
	texture_font_t  *font{};
	float           height{};
	std::string     lastError{};
	std::string     vertShader = "/* Freetype GL - A C OpenGL Freetype engine\n"
	                             " *\n"
	                             " * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying\n"
	                             " * file `LICENSE` for more details.\n"
	                             " */\n"
								 "#version 330\n"
								 "\n"
	                             "uniform mat4 MVP;\n"
	                             "\n"
	                             "in vec3 i_Position;\n"
	                             "in vec2 i_TextureCoords;\n"
	                             "in vec4 i_Color;\n"
								 "\n"
								 "out vec2 TextureCoords;\n"
								 "out vec4 Color;\n"
								 "\n"
	                             "void main()\n"
	                             "{\n"
	                             "    TextureCoords.xy = i_TextureCoords.xy;\n"
	                             "    Color            = i_Color;\n"
	                             "    gl_Position      = MVP * vec4(i_Position, 1.0);\n"
	                             "}";

	std::string fragShader = "/* Freetype GL - A C OpenGL Freetype engine\n"
	                         " *\n"
	                         " * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying\n"
	                         " * file `LICENSE` for more details.\n"
	                         " */\n"
	                         "#version 330\n"
							 "\n"
	                         "uniform sampler2D u_Texture0;\n"
							 "\n"
	                         "in vec2 TextureCoords;\n"
							 "in vec4 Color;\n"
							 "\n"
							 "out vec4 pixelColor;\n"
							 "\n"
	                         "void main()\n"
	                         "{\n"
	                         "    float a = texture2D(u_Texture0, TextureCoords.xy).r;\n"
	                         "    pixelColor = vec4(Color.rgb, Color.a*a);\n"
	                         "}";
};

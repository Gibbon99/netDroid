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
	                             "uniform mat4 MVP;\n"
	                             "\n"
	                             "attribute vec3 vertex;\n"
	                             "attribute vec2 tex_coord;\n"
	                             "attribute vec4 color;\n"
	                             "void main()\n"
	                             "{\n"
	                             "    gl_TexCoord[0].xy = tex_coord.xy;\n"
	                             "    gl_FrontColor     = color;\n"
	                             "    gl_Position       = MVP * vec4(vertex,1.0);\n"
	                             "}";

	std::string fragShader = "/* Freetype GL - A C OpenGL Freetype engine\n"
	                         " *\n"
	                         " * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying\n"
	                         " * file `LICENSE` for more details.\n"
	                         " */\n"
	                         "uniform sampler2D textureID;\n"
	                         "void main()\n"
	                         "{\n"
	                         "    float a = texture2D(textureID, gl_TexCoord[0].xy).r;\n"
	                         "    gl_FragColor = vec4(gl_Color.rgb, gl_Color.a*a);\n"
	                         "}";
};

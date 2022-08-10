#pragma once

#include <string>
#include "SDL2/SDL.h"
#include <SDL_image.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

class droidWindow
{

public:
	droidWindow (int newWindowWidth, int newWindowHeight);

	~droidWindow ();

	std::string getLinkedVersion ();

	std::string getCompiledVersion ();

	bool create (const std::string &windowName, bool useFullscreen);

	bool setWindowTitle (const std::string &newTitleText);

	SDL_Window *getWindow ();

	std::string getLastError ();

	int getWidth ();

	int getHeight ();

	// Present the frame to the screen
	void presentFrame ();

	// Start the drawing frame
	void startFrame ();

	bool createFrameBuffer ();

private:

	bool createFBOShaderProgram ();

	int           m_windowWidth{};
	int           m_windowHeight{};
	SDL_Window    *window{};                     // The window we are rendering to
	SDL_Surface   *surface{};
	SDL_Surface   *iconSurface{};
	SDL_GLContext openglContext{};
	uint          frameBufferID{};
	uint          frameBufferTextureID{};
	uint          frameBufferDepthID{};
	uint          fboVertexBufferID{};
	uint          fboShaderProgramID{};
	uint          fboTextureShaderID{};
	uint          fboVAO_ID{};
	std::string   lastError{};

	std::string fboVertShader = "#version 330 core\n"
	                            "\n"
	                            "// Input vertex data, different for all executions of this shader.\n"
	                            "layout(location = 0) in vec3 vertexPosition_modelspace;\n"
	                            "\n"
	                            "// Output data ; will be interpolated for each fragment.\n"
	                            "out vec2 UV;\n"
	                            "\n"
	                            "void main(){\n"
	                            "gl_Position =  vec4(vertexPosition_modelspace,1);\n"
	                            "UV = (vertexPosition_modelspace.xy+vec2(1,1))/2.0;\n"
	                            "}";

	std::string fboFragShader = "#version 330 core\n"
	                            "\n"
	                            "in vec2 UV;\n"
	                            "\n"
	                            "out vec3 color;\n"
	                            "\n"
	                            "uniform sampler2D renderedTexture;\n"
	                            "\n"
	                            "void main(){\n"
	                            "color = texture( renderedTexture, UV).xyz ;\n"
	                            "}";
};

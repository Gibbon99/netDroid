#include "../../hdr/classes/c_window.h"
#include "../../hdr/main.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Constructor
droidWindow::droidWindow (int newWindowWidth, int newWindowHeight)
//----------------------------------------------------------------------------------------------------------------------
{
	m_windowWidth  = newWindowWidth;
	m_windowHeight = newWindowHeight;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Get the compiled version of the window library
std::string droidWindow::getCompiledVersion ()
//----------------------------------------------------------------------------------------------------------------------
{
	SDL_version compiled;

	SDL_VERSION (&compiled);

	return sys_getString ("Compiled against SDL version [ %d.%d.%d ]", compiled.major, compiled.minor, compiled.patch);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Get the linked version of the window library
std::string droidWindow::getLinkedVersion ()
//----------------------------------------------------------------------------------------------------------------------
{
	SDL_version linked;

	SDL_GetVersion (&linked);

	return sys_getString ("Linked against SDL version [ %d.%d.%d ]", linked.major, linked.minor, linked.patch);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Create the shader program to use to render the framebuffer to screen
bool droidWindow::createFBOShaderProgram ()
//----------------------------------------------------------------------------------------------------------------------
{
	fboShaderProgramID = glCreateProgram ();

	auto vs = glCreateShader (GL_VERTEX_SHADER);
	glShaderSource (vs, 1, reinterpret_cast<const GLchar *const *>(&fboVertShader), nullptr);
	glCompileShader (vs);

	auto isCompiled = GL_FALSE;
	glGetShaderiv (vs, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv (vs, GL_INFO_LOG_LENGTH, &maxLength);
		//
		// The maxLength includes the NULL character
		char *error = (char *) malloc (maxLength);
		glGetShaderInfoLog (vs, maxLength, &maxLength, error);
		lastError = sys_getString ("FBO vertex shader error : [ %s ]", error);
		free (error);
		return false;
	}
	glAttachShader (fboShaderProgramID, vs);
	//
	// Fragment shader
	auto fs = glCreateShader (GL_FRAGMENT_SHADER);
	glShaderSource (fs, 1, reinterpret_cast<const GLchar *const *>(&fboFragShader), nullptr);
	glCompileShader (fs);

	isCompiled = GL_FALSE;
	glGetShaderiv (fs, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv (fs, GL_INFO_LOG_LENGTH, &maxLength);
		//
		// The maxLength includes the NULL character
		char *error = (char *) malloc (maxLength);
		glGetShaderInfoLog (fs, maxLength, &maxLength, error);
		lastError = sys_getString ("FBO fragment shader error : [ %s ]", error);
		free (error);
		return false;
	}
	glAttachShader (fboShaderProgramID, fs);
	glLinkProgram (fboShaderProgramID);

	glDeleteShader (vs);
	glDeleteShader (fs);

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Create the frame buffer to render to
bool droidWindow::createFrameBuffer ()
//----------------------------------------------------------------------------------------------------------------------
{
	glGenFramebuffers (1, &frameBufferID);
	glBindFramebuffer (GL_FRAMEBUFFER, frameBufferID);

	glGenTextures (1, &frameBufferTextureID);
	glBindTexture (GL_TEXTURE_2D, frameBufferTextureID);
	//
	// Create an empty textureID
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, m_windowWidth, m_windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	//
	// Set filtering
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//
	// Create depth buffer as well
	glGenRenderbuffers (1, &frameBufferDepthID);
	glBindRenderbuffer (GL_RENDERBUFFER, frameBufferDepthID);
	glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_windowWidth, m_windowHeight);
	glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBufferDepthID);
	//
	// Set the color attachment
	glFramebufferTexture (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, frameBufferTextureID, 0);
	GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers (1, drawBuffers);
	//
	// Check the framebuffer is ok
	GLenum checkStatus = glCheckFramebufferStatus (GL_FRAMEBUFFER);
	if (checkStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (checkStatus)
		{
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				lastError = "Not all framebuffer attachment points are framebuffer attachment complete. This means that at least one attachment point with a renderbuffer or textureID attached has its attached object no longer in existence or has an attached image with a width or height of zero, or the color attachment point has a non-color-renderable image attached, or the depth attachment point has a non-depth-renderable image attached, or the stencil attachment point has a non-stencil-renderable image attached.";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				lastError = "No images are attached to the framebuffer.";
				break;

			case GL_FRAMEBUFFER_UNSUPPORTED:
				lastError = "The combination of internal formats of the attached images violates an implementation-dependent set of restrictions.";
				break;

			default:
				lastError = sys_getString ("Failed to create framebuffer object - status [ 0x%x ]", checkStatus);
				break;
		}
		return false;
	}
	//
	// Create and compile our GLSL program from the shaders
	if (!createFBOShaderProgram ())
		return false;
	//
	// The fullscreen quad's FBO
	static const GLfloat fboQuadVertexData[] = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,};

	glGenVertexArrays (1, &fboVAO_ID);
	glBindVertexArray (fboVAO_ID);

	glUseProgram (fboShaderProgramID);

	glGenBuffers (1, &fboVertexBufferID);
	glBindBuffer (GL_ARRAY_BUFFER, fboVertexBufferID);
	glBufferData (GL_ARRAY_BUFFER, sizeof (fboQuadVertexData), fboQuadVertexData, GL_STATIC_DRAW);
	glVertexAttribPointer (0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	                       3,                  // size
	                       GL_FLOAT,           // type
	                       GL_FALSE,           // normalized?
	                       0,                  // stride
	                       (void *) 0          // array buffer offset
	);
	fboTextureShaderID = glGetUniformLocation (fboShaderProgramID, "renderedTexture");

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Create the window and imGui renderer
bool droidWindow::create (const std::string &windowName, bool useFullscreen)
//----------------------------------------------------------------------------------------------------------------------
{
	if (SDL_Init (SDL_INIT_EVERYTHING) < 0)
	{
		lastError = sys_getString ("ERROR : SDL could not initialize. SDL Error [ %s ]", SDL_GetError ());
		return false;
	}

	if (IMG_Init (IMG_INIT_JPG) < 0)
	{
		lastError = sys_getString ("ERROR : SDL_Image could not initialize. SDL Error [ %s ]", IMG_GetError ());
		return false;
	}

	iconSurface   = SDL_LoadBMP("data/icon.bmp");
	if (iconSurface != nullptr)
		SDL_SetWindowIcon (window, iconSurface);
	else
		lastError = sys_getString ("Error : Window : Unable to load window icon.");

	//
	// Create the window centered at resolution
	Uint32 windowFlags;

	windowFlags = SDL_WINDOW_OPENGL;
	if (useFullscreen)
		windowFlags |= SDL_WINDOW_FULLSCREEN;

	window = SDL_CreateWindow (windowName.c_str (), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight, windowFlags);

	// Was the window created ok
	if (!window)
	{
		lastError = sys_getString ("Unable to create window : [ %s ]", SDL_GetError ());
		return false;
	}

	/*
	// TODO: Parse the modes and get the refresh rate
	if ( fullScreen )
	{
		SDL_DisplayMode setDisplayMode;
		setDisplayMode.refresh_rate = 75;
		setDisplayMode.w = winWidth;
		setDisplayMode.h = winHeight;
		setDisplayMode.format = SDL_PIXELFORMAT_RGB888;
		if ( SDL_SetWindowDisplayMode (mainWindow, &setDisplayMode) != 0 )
		{
			con_print (CON_ERROR, true, "Unable to set displaymode [ %s ]", SDL_GetError ());
			printf ("Display error [ %s ]\n", SDL_GetError ());
			sys_shutdownToSystem ();
		}
	}
*/

	// Set the OpenGL version.
	// SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute (SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

#if (DEBUG_LEVEL > 0)
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	// Create the opengl context and attach it to the window
	openglContext    = SDL_GL_CreateContext (window);
	if (nullptr == openglContext)
	{
		lastError = sys_getString ("Unable to create OpenGL context : [ %s ]", SDL_GetError ());
		return false;
	}

	if (SDL_GL_MakeCurrent (window, openglContext) < 0)
	{
		lastError = sys_getString ("Context error [ %s ]", SDL_GetError ());
		return false;
	}
	//
	// Init the OpenGL extensions
	glewExperimental = GL_TRUE;
	auto glewReturnCode = glewInit ();
	if (GLEW_OK != glewReturnCode)
	{
		lastError = sys_getString ("Failed to start OpenGL extensions : [ %s ]", glewGetErrorString (glewReturnCode));
		return false;
	}

	// Turn on double buffering with a 24bit Z buffer.
	// May need to change this to 16 or 32
	SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
	if (-1 == SDL_GL_SetSwapInterval (1))
	{
		lastError = sys_getString ("Unable to use selected vsync method : [ %s ]", SDL_GetError ());
		SDL_GL_SetSwapInterval (1);
	}

	if (!createFrameBuffer ())
	{
		return false;
	}

	/*
	int value = 0;

	SDL_GL_GetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, &value);
	con_print (CON_INFO, true, "SDL_GL_CONTEXT_MAJOR_VERSION : %i ", value);

	SDL_GL_GetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, &value);
	con_print (CON_INFO, true, "SDL_GL_CONTEXT_MINOR_VERSION: %i ", value);

	SDL_GL_GetAttribute (SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, &value);
	con_print (CON_INFO, true, "SDL_GL_FRAMEBUFFER_SRGB_CAPABLE: %i :", value);
*/
	return true;
}

/*
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext ();
	ImGuiIO &io = ImGui::GetIO ();
	(void) io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	if (!io.Fonts->AddFontFromFileTTF (fontName.c_str(), 18.0f))
	{
		m_lastError = sys_getString("Error : Could not load font file [ %s ]", fontName.c_str());
		return false;
	}
	// Build atlas
	io.Fonts->Build();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark ();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer (window);
	ImGui_ImplSDLRenderer_Init (renderer);

*/


//----------------------------------------------------------------------------------------------------------------------
//
// Return the last error message
std::string droidWindow::getLastError ()
//----------------------------------------------------------------------------------------------------------------------
{
	return lastError;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Deconstructor
droidWindow::~droidWindow ()
//----------------------------------------------------------------------------------------------------------------------
{
	//Destroy the window
	SDL_DestroyWindow (window);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the window handle
SDL_Window *droidWindow::getWindow ()
//----------------------------------------------------------------------------------------------------------------------
{
	return window;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Start the drawing frame
void droidWindow::startFrame ()
//----------------------------------------------------------------------------------------------------------------------
{
	//
	// Bind the framebuffer
	glBindFramebuffer (GL_FRAMEBUFFER, frameBufferID);

	glClearColor (0.0f, 0.05f, 0.0f, 0.0f);
	glViewport (0, 0, m_windowWidth, m_windowHeight);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Start the Dear ImGui frame
//	ImGui_ImplSDLRenderer_NewFrame ();
//	ImGui_ImplSDL2_NewFrame (window);
//	ImGui::NewFrame ();
}

//----------------------------------------------------------------------------------------------------------------------
//
// Present the frame to the screen
void droidWindow::presentFrame ()
//----------------------------------------------------------------------------------------------------------------------
{
//	ImGui::Render ();
//	ImGui_ImplSDLRenderer_RenderDrawData (ImGui::GetDrawData ());
	//
	// Turn off framebuffer
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	glViewport (0, 0, m_windowWidth, m_windowHeight);
	//
	// Clear the screen
//	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // TODO not needed with full screen framebuffer blit
	//
	// Use the shader for the fullscreen framebuffer
	glUseProgram (fboShaderProgramID);
	//
	// Bind the framebuffer textureID to unit 0
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, frameBufferTextureID);
	//
	// Set the fragment shader to use textureID 0
	glUniform1i (fboTextureShaderID, 0);

	glBindVertexArray (fboVAO_ID);
	//
	// Attribute buffer : vertices
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, fboVertexBufferID);
	// Draw the framebuffer
	glDrawArrays (GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	glDisableVertexAttribArray (0);
	glUseProgram (0);

	SDL_GL_SwapWindow (window);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Update the window title
bool droidWindow::setWindowTitle (const std::string &newTitleText)
//----------------------------------------------------------------------------------------------------------------------
{
	SDL_SetWindowTitle (window, newTitleText.c_str ());

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the window width
int droidWindow::getWidth ()
//----------------------------------------------------------------------------------------------------------------------
{
	return m_windowWidth;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the window height
int droidWindow::getHeight ()
//----------------------------------------------------------------------------------------------------------------------
{
	return m_windowHeight;
}
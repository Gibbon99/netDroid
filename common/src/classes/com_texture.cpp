#include <SDL_system.h>
#include <string>
#include <SDL_image.h>
#include "com_texture.h"
#include "com_util.h"
#include "../../../client/hdr/openGL/c_glDebug.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Constructor - crete a debug texture by default
droidTexture::droidTexture()
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
//
// Free the memory used by the textureID and the surface
droidTexture::~droidTexture ()
//----------------------------------------------------------------------------------------------------------------------
{
	if (surface != nullptr)
		SDL_FreeSurface (surface);

	if (textureID != 0)
	{
		textureState = NOT_LOADED;
		glDeleteTextures (1, &textureID);
		textureID = 0;
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Create a checkboard pattern texture
// From: https://stackoverflow.com/questions/3569261/multiple-textures-in-glsl-only-one-works
void droidTexture::makeCheckTex(int textureSize)
//----------------------------------------------------------------------------------------------------------------------
{
	GLubyte image[textureSize][textureSize][4]; // RGBA storage

	for (int i = 0; i < textureSize; i++)
	{
		for (int j = 0; j < textureSize; j++)
		{
			int c = (((i & 0x8) == 0) ^ (((j & 0x8)) == 0)) * 255;
			image[i][j][0] = (GLubyte) c;
			image[i][j][1] = (GLubyte) c;
			image[i][j][2] = (GLubyte) c;
			image[i][j][3] = (GLubyte) 255;
		}
	}

	GLuint texName;
	glGenTextures(1, &texName);
	glActiveTexture ( GL_TEXTURE0 ) ;
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize, textureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	textureID = texName;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Load an image file from disk
bool droidTexture::loadFromDisk (const std::string &newFileName)
//----------------------------------------------------------------------------------------------------------------------
{
	surfaceState = SURFACE_LOADING;
	surface      = IMG_Load (newFileName.c_str ());
	if (nullptr == surface)
	{
		lastError    = sys_getString ("ERROR : Unable to load [ %s ] - [ %s ]", newFileName.c_str (), IMG_GetError ());
		surfaceState = NOT_LOADED;
		textureState = NOT_LOADED;
		return false;
	}
	surfaceState = SURFACE_LOADED;
	lastError = "";
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Convert a loaded surface image to an openGL textureID
bool droidTexture::convertToTexture ()
//----------------------------------------------------------------------------------------------------------------------
{
	//
	// Free if this texture is already loaded
	if (textureID != 0)
	{
		textureState = NOT_LOADED;
		glDeleteTextures (1, &textureID);
		textureID = 0;
	}
	//
	// Make sure the surface is still usable
	if (nullptr == surface)
	{
		lastError = sys_getString("[ %s ] Invalid surface pointer in [ %s ] at [ %i ]", imageName.c_str(), __FILE__, __LINE__);
		return false;
	}
	//
	// Make sure the surface is valid
	if (nullptr == surface->format)
	{
		lastError = sys_getString("[ %s] Invalid format pointer in [ %s ] at [ %i ]", imageName.c_str(), __FILE__, __LINE__);
		return false;
	}

	auto colorCount = surface->format->BytesPerPixel;
	if (colorCount == 4)
	{
		if (surface->format->Rmask == 0x000000ff)
			format = GL_RGBA;
		else
			format = GL_BGRA;
	}
	else if (colorCount == 3)
	{
		if (surface->format->Rmask == 0x000000ff)
			format = GL_RGB;
		else
			format = GL_BGR;
	}
	else
	{
		lastError = sys_getString("Image [ %s ] has invalid bytesPerPixel value. [ %s ] at [ %i ]", imageName.c_str(), __FILE__, __LINE__);
		return false;
	}

	width  = surface->w;
	height = surface->h;
	if ((0 == width) || (0 == height))
	{
		lastError = sys_getString("Image [ %s ] width or height is invalid. [ %s ] at [ %i ]", imageName.c_str(), __FILE__, __LINE__);
		return false;
	}
	//
	// Get new ID for this texture
	glGenTextures (1, &textureID);
	auto openGLResult = glGetError();
	if (GL_NO_ERROR != openGLResult)
	{
		lastError = sys_getString("Image [ %s ] glGenTextures failed with [ %s ]. [ %s ] at [ %i ]", imageName.c_str(), getGLErrorString(openGLResult).c_str(), __FILE__, __LINE__);
		return false;
	}
	//
	// Start workng with it
	glBindTexture (GL_TEXTURE_2D, textureID);
	openGLResult = glGetError();
	if (GL_NO_ERROR != openGLResult)
	{
		lastError = sys_getString("Image [ %s ] glBindTexture failed with [ %s ]. [ %s ] at [ %i ]", imageName.c_str(), getGLErrorString(openGLResult).c_str(), __FILE__, __LINE__);
		return false;
	}
	//
	// Create the texture for it
	glTexImage2D (GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
	openGLResult = glGetError();
	if (GL_NO_ERROR != openGLResult)
	{
		lastError = sys_getString("Image [ %s ] glTexImage2D failed with [ %s ]. [ %s ] at [ %i ]", imageName.c_str(), getGLErrorString(openGLResult).c_str(), __FILE__, __LINE__);
		return false;
	}
	//
	// Set filtering
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	textureState = TEXTURE_LOADED;
	lastError = "";
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the textureID width
int droidTexture::getWidth ()
//----------------------------------------------------------------------------------------------------------------------
{
	return width;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the textureID height
int droidTexture::getHeight ()
//----------------------------------------------------------------------------------------------------------------------
{
	return height;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the textureID format
Uint32 droidTexture::getFormat ()
//----------------------------------------------------------------------------------------------------------------------
{
	return format;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return if the textureID is ready to use or not
uint droidTexture::getTextureState ()
//----------------------------------------------------------------------------------------------------------------------
{
	return textureState;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return if the SDL_Surface has been loaded is ready to use or not
uint droidTexture::getSurfaceState ()
//----------------------------------------------------------------------------------------------------------------------
{
	return surfaceState;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set if the texture has been loaded or not
void droidTexture::setTextureState (int newTextureState)
//----------------------------------------------------------------------------------------------------------------------
{
	textureState = newTextureState;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set if the surface has been loaded or not
void droidTexture::setSurfaceState (int newSurfaceState)
//----------------------------------------------------------------------------------------------------------------------
{
	surfaceState = newSurfaceState;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the textureID
uint droidTexture::getTextureID ()
//----------------------------------------------------------------------------------------------------------------------
{
	return textureID;
}


//----------------------------------------------------------------------------------------------------------------------
//
// Flip the SDL_Surface to reflect the opengl coordinates used
//https://stackoverflow.com/questions/65815332/flipping-a-surface-vertically-in-sdl2
void droidTexture::flipSurface()
//----------------------------------------------------------------------------------------------------------------------
{
	SDL_LockSurface(surface);

	int  pitch   = surface->pitch;          // row size
	char *temp   = new char[pitch];         // intermediate buffer
	char *pixels = (char *) surface->pixels;

	for (int i = 0; i < surface->h / 2; ++i)
	{
		//
		// get pointers to the two rows to swap
		char *row1 = pixels + i * pitch;
		char *row2 = pixels + (surface->h - i - 1) * pitch;

		// swap rows
		memcpy(temp, row1, pitch);
		memcpy(row1, row2, pitch);
		memcpy(row2, temp, pitch);
	}

	delete[] temp;

	SDL_UnlockSurface(surface);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set the SDL_surface pointer
void droidTexture::setSurface (SDL_Surface *newSurface, bool doFlipSurface)
//----------------------------------------------------------------------------------------------------------------------
{
	if (surface != nullptr)
	{
		SDL_FreeSurface (surface);
		surface = nullptr;
	}

	surface = newSurface;

	if (doFlipSurface)
	{
		flipSurface();
	}
	surfaceState = SURFACE_LOADED;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the SDL_Surface pointer
SDL_Surface *droidTexture::getSurface()
//----------------------------------------------------------------------------------------------------------------------
{
	if (surfaceState == SURFACE_LOADED)
		return surface;
	else
		return nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set the image name for this texture
void droidTexture::setName(std::string newName)
//----------------------------------------------------------------------------------------------------------------------
{
	imageName = std::move(newName);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the current error string
std::string droidTexture::getLastError()
//----------------------------------------------------------------------------------------------------------------------
{
	if (!lastError.empty())
		return lastError;
	else
		return "No error.";
}
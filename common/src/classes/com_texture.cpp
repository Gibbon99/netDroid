#include <SDL_system.h>
#include <string>
#include <SDL_image.h>
#include <GL/gl.h>
#include "com_texture.h"
#include "com_util.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Free the memory used by the textureID and the surface
droidTexture::~droidTexture ()
//----------------------------------------------------------------------------------------------------------------------
{
	if (surface != nullptr)
		SDL_FreeSurface (surface);
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
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Convert a loaded surface image to an openGL textureID
bool droidTexture::convertToTexture ()
//----------------------------------------------------------------------------------------------------------------------
{
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

	width  = surface->w;
	height = surface->h;

	glGenTextures (1, &textureID);
	if (GL_INVALID_VALUE == textureID)  // Need to get the error first before checking glGetError() ?
	{
		lastError = "Unable to generate a texture name.";
		return false;
	}
	glBindTexture (GL_TEXTURE_2D, textureID);
	glTexImage2D (GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
	//
	// Set filtering
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	textureState = TEXTURE_LOADED;

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
int droidTexture::getFormat ()
//----------------------------------------------------------------------------------------------------------------------
{
	return format;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the textureID access
int droidTexture::getAccess ()
//----------------------------------------------------------------------------------------------------------------------
{
	return access;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return if the textureID is ready to use or not
int droidTexture::getTextureState ()
//----------------------------------------------------------------------------------------------------------------------
{
	return textureState;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return if the SDL_Surface has been loaded is ready to use or not
int droidTexture::getSurfaceState ()
//----------------------------------------------------------------------------------------------------------------------
{
	return surfaceState;
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
// Set the SDL_surface pointer
void droidTexture::setSurface (SDL_Surface *newSurface)
//----------------------------------------------------------------------------------------------------------------------
{
	surface = std::move(newSurface); //SDL_ConvertSurface (newSurface, newSurface->format, 0);

	printf("*** Bytes per pixel after assign : %i \n", surface->format->BytesPerPixel);

	surfaceState = SURFACE_LOADED;
	// if textureloaded - free it so it can be reloaded
}
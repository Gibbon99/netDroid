#include <SDL_system.h>
#include <string>
#include <SDL_image.h>
#include <GL/gl.h>
#include "com_texture.h"
#include "com_util.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Free the memory used by the texture and the surface
droidTexture::~droidTexture()
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
// Convert a loaded surface image to an openGL texture
bool droidTexture::convertToTexture (SDL_Surface *newSurface)
//----------------------------------------------------------------------------------------------------------------------
{
	textureState = TEXTURE_LOADING;
	surfaceState = SURFACE_LOADING;

	auto colorCount = newSurface->format->BytesPerPixel;
	if (colorCount == 4)
	{
		if (newSurface->format->Rmask == 0x000000ff)
			format = GL_RGBA;
		else
			format = GL_BGRA;
	}
	else if (colorCount == 3)
	{
		if (newSurface->format->Rmask == 0x000000ff)
			format = GL_RGB;
		else
			format = GL_BGR;
	}

	width  = newSurface->w;
	height = newSurface->h;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, newSurface->pixels);
	//
	// Set filtering
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	textureState = TEXTURE_LOADED;
	surfaceState = SURFACE_LOADED;

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the texture width
int droidTexture::getWidth ()
//----------------------------------------------------------------------------------------------------------------------
{
	return width;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the texture height
int droidTexture::getHeight ()
//----------------------------------------------------------------------------------------------------------------------
{
	return height;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the texture format
int droidTexture::getFormat ()
//----------------------------------------------------------------------------------------------------------------------
{
	return format;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the texture access
int droidTexture::getAccess ()
//----------------------------------------------------------------------------------------------------------------------
{
	return access;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the textureID
uint droidTexture::getTextureID ()
//----------------------------------------------------------------------------------------------------------------------
{
	return texture;
}
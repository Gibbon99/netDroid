#pragma once

#include <cstdlib>
#include <vector>
#include <string>
#include <SDL.h>

enum TEXTURE_STATE
{
	NOT_LOADED = 0,
	SURFACE_LOADING,
	SURFACE_LOADED,
	TEXTURE_LOADING,
	TEXTURE_LOADED
};

class droidTexture
{
public:

	~droidTexture ();

	bool loadFromDisk (const std::string &fileName);

	bool convertToTexture (SDL_Surface *newSurface);

	int getWidth ();

	int getHeight ();

	int getAccess ();

	int getFormat ();

	uint getTextureID ();

private:

	uint        textureState = NOT_LOADED;
	uint        surfaceState = NOT_LOADED;
	int         width{};
	int         height{};
	Uint32      format{};
	int         access{};
	SDL_Surface *surface{nullptr};
	uint        texture{};
	std::string lastError{};
	std::string fileName{};
};
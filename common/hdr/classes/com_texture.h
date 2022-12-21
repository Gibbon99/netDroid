#pragma once

#include <cstdlib>
#include <vector>
#include <string>
#include <SDL2/SDL.h>

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
	droidTexture ();

	void makeCheckTex(int textureSize);

	bool loadFromDisk (const std::string &fileName);
	bool convertToTexture ();
	void flipSurface();

	int getWidth ();
	int getHeight ();
	Uint32 getFormat ();
	uint getTextureState ();
	uint getSurfaceState ();
	uint getTextureID ();
	SDL_Surface *getSurface();
	std::string getLastError();

	void setTextureState (int newTextureState);
	void setSurfaceState (int newSurfaceState);
	void setName(std::string newName);

	void setSurface (SDL_Surface *newSurface, bool doFlipSurface);

private:

	uint        textureState = NOT_LOADED;
	uint        surfaceState = NOT_LOADED;
	int         width{};
	int         height{};
	Uint32      format{};
	SDL_Surface *surface{nullptr};
	uint        textureID{};
	std::string lastError{};
	std::string fileName{};
	std::string imageName{};
};
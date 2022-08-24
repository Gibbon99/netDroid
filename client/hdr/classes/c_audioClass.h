#pragma once

#include <string>
#include <vector>
#include <SDL_mixer.h>

class droidAudioStruct
{
public:
	droidAudioStruct ();

	void setChunk (Mix_Chunk *newChunk);

	Mix_Chunk *getChunk () const;

	std::string getAudioName ();

	void setAudioName (const std::string &newAudioName);

	void setPlaying (bool newState);

	void setLoaded (bool newState);

	bool isLoaded ();

	bool isPlaying ();

private:
	bool        isSamplePlaying{false};
	bool        isSampleLoaded{false};
	Mix_Chunk   *audioChunk{};
	std::string audioName{};
};

class droidAudio
{

public:
	bool init ();

	bool play (std::string_view audioName, bool loop);

	bool load (std::string_view audioName, Mix_Chunk *newAudioChunk);

	std::string getLastError ();

private:

	std::string                   lastError{};
	std::vector<droidAudioStruct> audioFile{};
};

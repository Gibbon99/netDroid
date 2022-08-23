#include "../../hdr/classes/c_audioClass.h"
#include "com_util.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Constructor for this audio file
droidAudioStruct::droidAudioStruct()
//----------------------------------------------------------------------------------------------------------------------
{
	audioChunk = nullptr;
	isSamplePlaying = false;
	isSampleLoaded = false;
};

//----------------------------------------------------------------------------------------------------------------------
//
// Load a chunk and move it to here
void droidAudioStruct::setChunk (Mix_Chunk *newChunk)
//----------------------------------------------------------------------------------------------------------------------
{
	audioChunk = newChunk;

}

//----------------------------------------------------------------------------------------------------------------------
//
// Return a pointer to the audio chunk
Mix_Chunk * droidAudioStruct::getChunk() const
//----------------------------------------------------------------------------------------------------------------------
{
	if (audioChunk != nullptr)
		return audioChunk;

	return nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the last error string
std::string droidAudio::getLastError()
//----------------------------------------------------------------------------------------------------------------------
{
	return lastError;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Init the Audio system
bool droidAudio::init()
//----------------------------------------------------------------------------------------------------------------------
{
	if (Mix_Init(MIX_INIT_MP3) < 0)
	{
		lastError = SDL_GetError();
		return false;
	}

	if (Mix_OpenAudio(48000, AUDIO_F32SYS, 2, 2048) < 0)
	{
		lastError = SDL_GetError();
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set the name of the audio sample
void droidAudioStruct::setAudioName(const std::string& newAudioName)
//----------------------------------------------------------------------------------------------------------------------
{
	audioName = newAudioName;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set the name of the audio sample
std::string droidAudioStruct::getAudioName()
//----------------------------------------------------------------------------------------------------------------------
{
	return audioName;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set the playing state of the sample
void droidAudioStruct::setPlaying(bool newState)
//----------------------------------------------------------------------------------------------------------------------
{
	isSamplePlaying = newState;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set the loaded state of the sample
void droidAudioStruct::setLoaded(bool newState)
//----------------------------------------------------------------------------------------------------------------------
{
	isSampleLoaded = newState;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Is the sample loaded
bool droidAudioStruct::isLoaded()
//----------------------------------------------------------------------------------------------------------------------
{
	return isSampleLoaded;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Is the sample playing
bool droidAudioStruct::isPlaying()
//----------------------------------------------------------------------------------------------------------------------
{
	return isSamplePlaying;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Play on the next available channel
bool droidAudio::play(std::string audioName, bool loop)
//----------------------------------------------------------------------------------------------------------------------
{
	for (auto &audioItr: audioFile)
	{
		if (audioItr.getAudioName () == audioName)
		{
			if (audioItr.isLoaded() == true)
			{
				if (-1 == Mix_PlayChannel (-1, audioItr.getChunk (), loop))
				{
					lastError = sys_getString("Unable to play audio : [ %s ]", Mix_GetError());
					return false;
				}
				audioItr.setPlaying (true);
				return true;
			}
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Load an audio chunk into audioNamed slot
bool droidAudio::load(const std::string& audioName, Mix_Chunk *newAudioChunk)
//----------------------------------------------------------------------------------------------------------------------
{
	droidAudioStruct tempAudioFile{};

	tempAudioFile.setAudioName(audioName);
	tempAudioFile.setChunk (newAudioChunk);
	tempAudioFile.setPlaying(false);
	tempAudioFile.setLoaded(true);

	for (auto &audioItr : audioFile)
	{
		if (audioItr.getAudioName() == audioName)
		{
			lastError = sys_getString("Audio sample [ %s ] is already loaded.", audioName.c_str());
			return false;
		}
	}
	//
	// Not found so add it
	audioFile.push_back(tempAudioFile);
	return true;
}
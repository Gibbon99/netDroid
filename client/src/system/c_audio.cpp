#include <SDL_rwops.h>
#include "../../hdr/system/c_audio.h"
#include "../../hdr/main.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Convert a datapacket image to an SDL audio ready for playing on the client
void c_convertPacketToAudio (dataPacket newDataPacket)
//----------------------------------------------------------------------------------------------------------------------
{
	SDL_RWops *rw = SDL_RWFromMem (static_cast<void *>(&newDataPacket.binaryData[0]), newDataPacket.binarySize);
	if (nullptr == rw)
	{
		clientTextures[newDataPacket.testString].setSurfaceState (NOT_LOADED);
		clientTextures[newDataPacket.testString].setTextureState (NOT_LOADED);
		clientMessage.message (MESSAGE_TARGET_STD_OUT, sys_getString ("Couldn't create RWops [ %s ]", SDL_GetError ()));
		return;
	}

	auto tempChunk = Mix_LoadWAV_RW (rw, 0);
	if (nullptr == tempChunk)
	{
		clientMessage.message(MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_LOGFILE, sys_getString("Unable to load audio [ %s ] ; [ %s ]", newDataPacket.testString.c_str(), Mix_GetError()));
		return;
	}

	clientAudio.load (newDataPacket.testString, Mix_LoadWAV_RW (rw, 0));
}

//----------------------------------------------------------------------------------------------------------------------
//
// Play a sample
void c_playSample(const std::string& sampleName, bool loop)
//----------------------------------------------------------------------------------------------------------------------
{
	clientAudio.play (sampleName, loop);
}
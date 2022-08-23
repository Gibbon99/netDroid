#pragma once

#include "com_globals.h"

// Convert a datapacket image to an SDL audio ready for playing on the client
void c_convertPacketToAudio (dataPacket newDataPacket);

// Play a sample
void c_playSample(const std::string& sampleName, bool loop);
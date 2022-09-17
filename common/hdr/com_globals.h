#pragma once

#include <string>
#include <enet/enet.h>
#include <simpleIni.h>
#include <pods.h>
#include <pods/msgpack.h>
#include <pods/buffers.h>
#include "com_crc.h"

#define TICKS_PER_SECOND 30.0f
#define MAX_FRAMESKIP 5
#define THREAD_DELAY_MS 10

enum class DATA_PACKET_TYPES
{
	PACKET_NEW_CLIENT_ID = 55,
	PACKET_NEW_USERNAME,
	PACKET_IMAGE,
	PACKET_AUDIO,
	PACKET_SHADER,
	PACKET_SCRIPT,
	PACKET_REQUEST_INIT_SCRIPT,
	PACKET_INIT_SCRIPT
};

typedef struct peerInformation
{
	int         ID{};
	std::string userName{};
	ENetPeer    peerInfo;
} peerInformation;

struct dataPacket
{
	DATA_PACKET_TYPES   packetType{};
	int                 clientID{};
	int                 packetData{};
	int                 binarySize{};
	std::string         packetString{};
	std::vector<int8_t> binaryData{};
	uint32_t            crc;

	PODS_SERIALIZABLE(PODS_MDR (packetType), PODS_MDR (clientID), PODS_MDR (packetData), PODS_MDR (binarySize), PODS_MDR (packetString), PODS_MDR (binaryData), PODS_MDR(crc))
};


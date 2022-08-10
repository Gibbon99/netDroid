#pragma once

#include <string>
#include <enet/enet.h>
#include <simpleIni.h>
#include <pods.h>
#include <pods/msgpack.h>
#include <pods/buffers.h>

#define TICKS_PER_SECOND 30.0f
#define MAX_FRAMESKIP 5
#define THREAD_DELAY_MS 10

enum class DATA_PACKET_TYPES
{
	PACKET_NEW_CLIENT_ID = 55,
	PACKET_NEW_USERNAME,
	PACKET_IMAGE,
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
	int                 packetData{};
	std::string         testString{};
	int                 binarySize{};
	std::vector<int8_t> binaryData{};

	PODS_SERIALIZABLE(PODS_MDR (packetType), PODS_MDR (packetData), PODS_MDR (testString), PODS_MDR (binarySize), PODS_MDR (binaryData))
};


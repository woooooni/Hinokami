#pragma once
#include <Network/ThreadManager.h>
#include <Network/Session.h>
#include <Network/SendBuffer.h>
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_S_LOGIN = 1000,
	PKT_S_OPEN_LEVEL = 1001,
	PKT_S_CREATE_OBJECT = 1002,
	PKT_S_WORLD_MATRIX = 1003,
	PKT_S_CHANGE_ANIMATION = 1004,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);

class C_TestPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::S_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_S_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_OPEN_LEVEL& pkt) { return MakeSendBuffer(pkt, PKT_S_OPEN_LEVEL); }
	static SendBufferRef MakeSendBuffer(Protocol::S_CREATE_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_S_CREATE_OBJECT); }
	static SendBufferRef MakeSendBuffer(Protocol::S_WORLD_MATRIX& pkt) { return MakeSendBuffer(pkt, PKT_S_WORLD_MATRIX); }
	static SendBufferRef MakeSendBuffer(Protocol::S_CHANGE_ANIMATION& pkt) { return MakeSendBuffer(pkt, PKT_S_CHANGE_ANIMATION); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = SendBufferManager::GetInstance()->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};
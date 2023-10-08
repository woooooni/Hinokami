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
	PKT_C_LOGIN = 1001,
	PKT_S_ENTER_LEVEL = 1002,
	PKT_C_ENTER_LEVEL = 1003,
	PKT_S_EXIT_LEVEL = 1004,
	PKT_C_EXIT_LEVEL = 1005,
	PKT_S_CREATE_OBJECT = 1006,
	PKT_C_CREATE_OBJECT = 1007,
	PKT_S_DELETE_OBJECT = 1008,
	PKT_C_DELETE_OBJECT = 1009,
	PKT_S_WORLD_MATRIX = 1010,
	PKT_C_WORLD_MATRIX = 1011,
	PKT_S_CHANGE_ANIMATION = 1012,
	PKT_C_CHANGE_ANIMATION = 1013,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt);
bool Handle_S_ENTER_LEVEL(PacketSessionRef& session, Protocol::S_ENTER_LEVEL& pkt);
bool Handle_S_EXIT_LEVEL(PacketSessionRef& session, Protocol::S_EXIT_LEVEL& pkt);
bool Handle_S_CREATE_OBJECT(PacketSessionRef& session, Protocol::S_CREATE_OBJECT& pkt);
bool Handle_S_DELETE_OBJECT(PacketSessionRef& session, Protocol::S_DELETE_OBJECT& pkt);
bool Handle_S_WORLD_MATRIX(PacketSessionRef& session, Protocol::S_WORLD_MATRIX& pkt);
bool Handle_S_CHANGE_ANIMATION(PacketSessionRef& session, Protocol::S_CHANGE_ANIMATION& pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_S_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LOGIN>(Handle_S_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_S_ENTER_LEVEL] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ENTER_LEVEL>(Handle_S_ENTER_LEVEL, session, buffer, len); };
		GPacketHandler[PKT_S_EXIT_LEVEL] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_EXIT_LEVEL>(Handle_S_EXIT_LEVEL, session, buffer, len); };
		GPacketHandler[PKT_S_CREATE_OBJECT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CREATE_OBJECT>(Handle_S_CREATE_OBJECT, session, buffer, len); };
		GPacketHandler[PKT_S_DELETE_OBJECT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DELETE_OBJECT>(Handle_S_DELETE_OBJECT, session, buffer, len); };
		GPacketHandler[PKT_S_WORLD_MATRIX] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_WORLD_MATRIX>(Handle_S_WORLD_MATRIX, session, buffer, len); };
		GPacketHandler[PKT_S_CHANGE_ANIMATION] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CHANGE_ANIMATION>(Handle_S_CHANGE_ANIMATION, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::C_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_C_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::C_ENTER_LEVEL& pkt) { return MakeSendBuffer(pkt, PKT_C_ENTER_LEVEL); }
	static SendBufferRef MakeSendBuffer(Protocol::C_EXIT_LEVEL& pkt) { return MakeSendBuffer(pkt, PKT_C_EXIT_LEVEL); }
	static SendBufferRef MakeSendBuffer(Protocol::C_CREATE_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_C_CREATE_OBJECT); }
	static SendBufferRef MakeSendBuffer(Protocol::C_DELETE_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_C_DELETE_OBJECT); }
	static SendBufferRef MakeSendBuffer(Protocol::C_WORLD_MATRIX& pkt) { return MakeSendBuffer(pkt, PKT_C_WORLD_MATRIX); }
	static SendBufferRef MakeSendBuffer(Protocol::C_CHANGE_ANIMATION& pkt) { return MakeSendBuffer(pkt, PKT_C_CHANGE_ANIMATION); }

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
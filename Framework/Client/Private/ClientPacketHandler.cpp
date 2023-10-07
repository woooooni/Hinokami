#include "stdafx.h"
#include "ClientPacketHandler.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "Level_Loading.h"
#include "Network_Manager.h"
#include "ServerSession.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return true;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	
	return true;
}

bool Handle_C_ENTER_LEVEL(PacketSessionRef& session, Protocol::C_ENTER_LEVEL& pkt)
{
	
	return true;
}

bool Handle_C_CREATE_OBJECT(PacketSessionRef& session, Protocol::C_CREATE_OBJECT& pkt)
{

	return true;
}

bool Handle_C_DELETE_OBJECT(PacketSessionRef& session, Protocol::C_DELETE_OBJECT& pkt)
{
	return false;
}

bool Handle_C_CREATE_OBJECT(PacketSessionRef& session, Protocol::C_ENTER_LEVEL& pkt)
{
	
	return true;
}


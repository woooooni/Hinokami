#include "stdafx.h"
#include "ServerSession.h"
#include "Network_Manager.h"
#include "ClientPacketHandler.h"


CServerSession::CServerSession()
{
}

void CServerSession::OnConnected()
{
	Client::CNetwork_Manager::GetInstance()
		->Set_ServerSession(static_pointer_cast<CServerSession>(shared_from_this()));
}

void CServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = PacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId 대역 체크
	ClientPacketHandler::HandlePacket(session, buffer, len);
}

void CServerSession::OnSend(int32 len)
{
}

void CServerSession::OnDisconnected()
{
}

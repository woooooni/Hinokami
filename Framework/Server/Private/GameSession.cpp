#include "stdafx.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ServerPacketHandler.h"



CGameSession::CGameSession()
{
}


void CGameSession::OnConnected()
{
	CGameSessionManager::GetInstance()->Add(static_pointer_cast<CGameSession>(shared_from_this()));
	cout << to_string(GetSessionID()) + " is Connected" << endl;
}



void CGameSession::OnDisconnected()
{
	CGameSessionManager::GetInstance()->Remove(static_pointer_cast<CGameSession>(shared_from_this()));
}

void CGameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId 대역 체크
	ServerPacketHandler::HandlePacket(session, buffer, len);
}

void CGameSession::OnSend(int32 len)
{

}


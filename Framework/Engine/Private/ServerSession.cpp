#include "Network_Manager.h"
#include "ServerSession.h"
#include "GameInstance.h"



CServerSession::CServerSession()
{
}

void CServerSession::OnConnected()
{
	GAME_INSTANCE->Set_ServerSession(static_pointer_cast<CServerSession>(shared_from_this()));
}

void CServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = PacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
}

void CServerSession::OnSend(int32 len)
{

}

void CServerSession::OnDisconnected()
{
}

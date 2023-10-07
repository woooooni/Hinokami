#include "stdafx.h"
#include "ClientPacketHandler.h"
#include "Network_Manager.h"
#include "ServerSession.h"


USING(Client)
IMPLEMENT_SINGLETON(CNetwork_Manager)

CNetwork_Manager::CNetwork_Manager()
{
}

void CNetwork_Manager::Set_ServerSession(ServerSessionRef session)
{
	m_pServerSession = session;
	m_bConnected = true;
}

void CNetwork_Manager::Send(SendBufferRef sendBuffer)
{
	m_pServerSession->Send(sendBuffer);
}


void CNetwork_Manager::Free()
{
	__super::Free();
	m_pServerSession = nullptr;
}
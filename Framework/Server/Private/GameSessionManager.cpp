#include "stdafx.h"
#include "GameSessionManager.h"
#include "GameSession.h"


IMPLEMENT_SINGLETON(CGameSessionManager)

CGameSessionManager::CGameSessionManager()
{
}

void CGameSessionManager::Add(GameSessionRef session)
{
	WRITE_LOCK;
	_sessions.insert(session);
	session->SetSessionID(m_iSessionID++);
}

void CGameSessionManager::Remove(GameSessionRef session)
{
	WRITE_LOCK;
	_sessions.erase(session);
}

void CGameSessionManager::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (GameSessionRef session : _sessions)
	{
		session->Send(sendBuffer);
	}
}

void CGameSessionManager::Broadcast_One(SendBufferRef sendBuffer, uint32 sessionID)
{
	WRITE_LOCK;
	auto iter = find_if(_sessions.begin(), _sessions.end(), [&](GameSessionRef _session) {
		if (_session->GetSessionID() == sessionID)		
			return true;
		return false;
	});

	if (iter != _sessions.end())
		(*iter)->Send(sendBuffer);
}

void CGameSessionManager::BroadcastOthers(SendBufferRef sendBuffer, uint32 sessionID)
{
	WRITE_LOCK;
	for (GameSessionRef session : _sessions)
	{
		if (session->GetSessionID() == sessionID)
			continue;

		session->Send(sendBuffer);
	}
}

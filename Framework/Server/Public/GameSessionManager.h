#pragma once


#include "Base.h"
#include "Server_Defines.h"
#include "GameSession.h"

BEGIN(Server)

class CGameSession;
using GameSessionRef = shared_ptr<CGameSession>;

class CGameSessionManager : public CBase
{
	DECLARE_SINGLETON(CGameSessionManager)

private:
	CGameSessionManager();
	virtual ~CGameSessionManager() = default;

public:
	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void Broadcast(SendBufferRef sendBuffer);
	void Broadcast_One(SendBufferRef sendBuffer, uint32 sessionID);
	void BroadcastOthers(SendBufferRef sendBuffer, uint32 sessionID);

public:
	uint32 Get_SessionCount()
	{
		READ_LOCK;
		return _sessions.size();
	}

private:
	USE_LOCK
	set<GameSessionRef> _sessions;

private:
	uint32 m_iSessionID = 0;

};

END
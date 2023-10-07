#pragma once

#include "Server_Defines.h"
#include "Session.h"


BEGIN(Server)

class CGameSession : public PacketSession
{
public:
	CGameSession();
	virtual ~CGameSession() = default;

	USE_LOCK

public:
	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE * buffer, int32 len) override;
	virtual void OnSend(int32 len) override;

private:

	wstring		m_strCharacter;
	_uint		m_iPlayerID;
};

END
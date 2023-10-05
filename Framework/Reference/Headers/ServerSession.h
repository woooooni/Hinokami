#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)
class CServerSession : public PacketSession
{
public:
	CServerSession();
	~CServerSession() = default;

	USE_LOCK

public:
	virtual void OnConnected() override;
	virtual void OnRecvPacket(BYTE * buffer, int32 len) override;
	virtual void OnSend(int32 len) override;
	virtual void OnDisconnected() override;


};

END
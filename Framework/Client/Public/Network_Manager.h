#pragma once

#include "Base.h"


class CServerSession;
using ServerSessionRef = shared_ptr<CServerSession>;

BEGIN(Client)
class CNetwork_Manager : public CBase
{
	DECLARE_SINGLETON(CNetwork_Manager)


private:
	CNetwork_Manager();
	virtual ~CNetwork_Manager() = default;

public:
	void				Set_ServerSession(ServerSessionRef session);
	ServerSessionRef&	Get_ServerSession() { return m_pServerSession; }

	void				Send(SendBufferRef sendBuffer);
	bool				Is_Connected() { return m_bConnected; }


private:
	ServerSessionRef m_pServerSession;


private:
	bool m_bConnected = false;
	_int m_iGameID = -1;

public:
	virtual void Free() override;
};
END
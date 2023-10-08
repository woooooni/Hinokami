#pragma once

#include "Server_Defines.h"
#include "Base.h"


BEGIN(Server)

class CServerMain : public CBase
{
private:
	CServerMain();
	virtual ~CServerMain() = default;

public:
	HRESULT Initialize();
	void Start_Server();
	void Tick_Server();
	void ShutDown_Server() { m_bShutDown = false; }

public:
	_bool Is_ShutDown() { return m_bShutDown; }

//private:
//	IN_ADDR Get_MyIPAddress();

private:
	_bool m_bShutDown;


public:
	static CServerMain* Create();
	virtual void Free() override;
};

END
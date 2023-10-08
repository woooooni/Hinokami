#pragma once

#include "Server_Defines.h"
#include "Base.h"
BEGIN(Server)

class CServerObject_Manager : public CBase
{

	DECLARE_SINGLETON(CServerObject_Manager)

private:
	CServerObject_Manager();
	virtual ~CServerObject_Manager() = default;
	USE_LOCK

public:
	HRESULT Reserve_Manager();
	void Tick(_float fTimeDelta);

public:
	HRESULT Add_GameObject(Protocol::S_CREATE_OBJECT& pkt);
	// HRESULT Delete_GameObejct();
	const map<uint32, vector<class CServerObject*>>& Get_AllObjects(uint32 iLevelIndex) { READ_LOCK return m_pLevelLayerObjects[iLevelIndex]; }

public:
	map<uint32, vector<class CServerObject*>>* m_pLevelLayerObjects = { nullptr };


	
public:
	virtual void Free() override;
};

END
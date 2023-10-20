#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#include "Lock.h"

/* 사본 객체들을 리스트에 보관한다. */

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);

public:
	void Clear();
	

public:
	list<class CGameObject*>& Find_GameObjects() { return m_GameObjects; }
	CGameObject* Find_GameObject(const wstring& strObjectTag);
	CGameObject* Find_GameObject(_int iObjectID);

private:
	list<class CGameObject*>	m_GameObjects;
	USE_LOCK

public:
	static CLayer* Create();
	virtual void Free() override;
};

END
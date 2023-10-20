#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#include "Lock.h"

/* �纻 ��ü���� ����Ʈ�� �����Ѵ�. */

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
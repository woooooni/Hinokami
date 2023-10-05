#pragma once

#include "Base.h"

/* �纻 ��ü���� ����Ʈ�� �����Ѵ�. */

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

	USE_LOCK
public:
	HRESULT Initialize();
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	

public:
	list<class CGameObject*>& Find_GameObjects() { return m_GameObjects; }
	CGameObject* Find_GameObject(const wstring& strObjectTag);
	CGameObject* Find_GameObject(_int iObjectID);

private:
	list<class CGameObject*>	m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END
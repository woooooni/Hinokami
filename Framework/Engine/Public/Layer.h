#pragma once

#include "Base.h"

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
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	

public:
	list<class CGameObject*>& Find_GameObjects() { return m_GameObjects; }
	CGameObject* Find_GameObject(const wstring& strObjectTag);

private:
	list<class CGameObject*>	m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END
#pragma once

#include "GameObject.h"

/* Ŭ���̾���Ʈ���� ������ �پ��� ���ӿ�����Ʈ���� �θ𰡵ȴ�. */

BEGIN(Client)

class CCharacter abstract : public CGameObject
{
protected:
	/* ������ ������ �� */
	CCharacter(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, wstring strObjectTag);

	/* �纻�� ������ �� */
	CCharacter(const CGameObject& rhs); /* ���� ������. */
	virtual ~CCharacter() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


protected:
	virtual HRESULT Ready_Components() PURE;

public:
	virtual void Free() override;
};

END
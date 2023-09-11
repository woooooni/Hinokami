#pragma once

#include "GameObject.h"

/* 클라이엉ㄴ트에서 제작할 다양한 게임오브젝트들의 부모가된다. */

BEGIN(Client)

class CCharacter abstract : public CGameObject
{
protected:
	/* 원형을 생성할 때 */
	CCharacter(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, wstring strObjectTag);

	/* 사본을 생성할 때 */
	CCharacter(const CGameObject& rhs); /* 복사 생성자. */
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
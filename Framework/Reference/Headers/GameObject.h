#pragma once

#include "Base.h"

/* 클라이엉ㄴ트에서 제작할 다양한 게임오브젝트들의 부모가된다. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	/* 원형을 생성할 때 */
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CGameObject(const CGameObject& rhs); /* 복사 생성자. */
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
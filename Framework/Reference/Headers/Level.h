#pragma once

#include "Base.h"

/* 클라이언트 개발자가 정의한 레벨클래스들의 부모가 되는 클래스 .*/
/* 엔진에서 클라이언에 있는 레벨을 보관하거나 레벨의 함수를 호출 할 수 있게 된다. */

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual HRESULT Tick(_float fTimeDelta);
	virtual HRESULT LateTick(_float fTimeDelta);
	virtual HRESULT Render_Debug() { return S_OK; }


public:
	virtual HRESULT Enter_Level() PURE;
	virtual HRESULT Exit_Level() PURE;

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

protected:
	_float4 m_vFogColor = { .1f, .2f, 1.f, 1.f };
	_float2 m_vFogStartEnd = { 0.f, 70.f };
	class CRenderer* m_pRendererCom = nullptr;

public:
	virtual void Free() override;
};

END
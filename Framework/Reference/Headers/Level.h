#pragma once

#include "Base.h"

/* Ŭ���̾�Ʈ �����ڰ� ������ ����Ŭ�������� �θ� �Ǵ� Ŭ���� .*/
/* �������� Ŭ���̾� �ִ� ������ �����ϰų� ������ �Լ��� ȣ�� �� �� �ְ� �ȴ�. */

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
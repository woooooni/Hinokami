#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Loading_Icon final : public CUI
{
protected:
	CUI_Loading_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Loading_Icon(const CUI_Loading_Icon& rhs);
	virtual ~CUI_Loading_Icon() = default;

public:
	virtual HRESULT Initialize_Prototype(const UI_INFO& tInfo);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	virtual HRESULT Ready_Components() override;

private:
	HRESULT Bind_ShaderResources();

private:
	_float m_fAccTime = 0.f;
	_float m_fNextTime = 0.05f;
	_uint m_iTextureIndex = 0;

public:
	static CUI_Loading_Icon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const UI_INFO& tInfo);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Enmu_ToolTip final : public CUI
{
protected:
	CUI_Enmu_ToolTip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Enmu_ToolTip(const CUI_Enmu_ToolTip& rhs);
	virtual ~CUI_Enmu_ToolTip() = default;

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
	_uint m_iCurrTextureIndex = 0;
	_float m_fAccChangeTexture = 0.f;
	_float m_fChangeTextureTime = 0.25f;

public:
	static CUI_Enmu_ToolTip* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const UI_INFO& tInfo);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
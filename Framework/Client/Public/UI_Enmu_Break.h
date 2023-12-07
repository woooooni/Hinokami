#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Enmu_Break final : public CUI
{
protected:
	CUI_Enmu_Break(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Enmu_Break(const CUI_Enmu_Break& rhs);
	virtual ~CUI_Enmu_Break() = default;

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


public:
	static CUI_Enmu_Break* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const UI_INFO& tInfo);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
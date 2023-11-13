#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Logo_SelectBase final : public CUI
{
protected:
	CUI_Logo_SelectBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Logo_SelectBase(const CUI_Logo_SelectBase& rhs);
	virtual ~CUI_Logo_SelectBase() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();




protected:
	virtual void On_MouseEnter(_float fTimeDelta) override;
	virtual void On_Mouse(_float fTimeDelta) override;
	virtual void On_MouseExit(_float fTimeDelta) override;

protected:
	virtual HRESULT Ready_Components() override;

private:
	CTexture* m_pNonSelecetedTexture;
	CTexture* m_pSelecetedTexture;

private:
	HRESULT Bind_ShaderResources();

public:
	static CUI_Logo_SelectBase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END
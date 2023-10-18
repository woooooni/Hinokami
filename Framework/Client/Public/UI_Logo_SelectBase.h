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
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END
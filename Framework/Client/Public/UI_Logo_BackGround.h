#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Logo_BackGround final : public CUI
{
protected:
	CUI_Logo_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Logo_BackGround(const CUI_Logo_BackGround& rhs);
	virtual ~CUI_Logo_BackGround() = default;

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
	static CUI_Logo_BackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const UI_INFO& tInfo);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END
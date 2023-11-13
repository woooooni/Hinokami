#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Loading_BackGround final : public CUI
{
protected:
	CUI_Loading_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Loading_BackGround(const CUI_Loading_BackGround& rhs);
	virtual ~CUI_Loading_BackGround() = default;

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
	_uint m_iTextureIndex = 0;

public:
	static CUI_Loading_BackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const UI_INFO& tInfo);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_BattleEnd final : public CUI
{
protected:
	CUI_BattleEnd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BattleEnd(const CUI_BattleEnd& rhs);
	virtual ~CUI_BattleEnd() = default;

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
	_float m_fScaling = 2.f;
	_float m_fAccDeletion = 0.f;
	_float m_fDeletionTime = 5.f;
	_float m_fAlpha = 1.f;

public:
	static CUI_BattleEnd* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const UI_INFO& tInfo);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END
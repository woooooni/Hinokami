#pragma once

#include "Client_Defines.h"
#include "Pool.h"
#include "Base.h"

BEGIN(Engine)
class CEffect;
END

BEGIN(Client)
class CSkill_Manager : public CBase
{
	DECLARE_SINGLETON(CSkill_Manager)

private:
	CSkill_Manager();
	virtual ~CSkill_Manager() = default;

public:
	HRESULT Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strEffectPath);
	void	Tick(_float fTimeDelta);

public:
	HRESULT Generate_Effect(const wstring& strPrototypeEffectName, _matrix OffsetMatrix, _matrix WorldMatrix, _float fEffectDeletionTime, class CGameObject* pOwner = nullptr);

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;


private:
	HRESULT Ready_Proto_Effects(const wstring& strEffectPath);


public:
	virtual void Free() override;
};

END
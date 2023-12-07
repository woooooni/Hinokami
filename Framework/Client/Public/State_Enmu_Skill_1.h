#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CState_Enmu_Skill_1 final : public CState
{
private:
	CState_Enmu_Skill_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine);
	virtual ~CState_Enmu_Skill_1() = default;

public:
	virtual HRESULT Initialize(const list<wstring>& AnimationList) override;

public:
	virtual void Enter_State(void* pArg = nullptr) override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CBoss_Enmu* m_pOwnerBoss = nullptr;
	class CGameObject* m_pTarget = nullptr;


private:
	class CUI_Enmu_Break* m_pBreakUI = nullptr;
	class CUI_Enmu_ToolTip* m_pToolTipUI = nullptr;

private:
	_uint m_iCurrCount = 0;
	_uint m_iKeyTabCount = 30;
	_bool m_bShoot = false;
	_bool m_bHit = false;

private:
	_float m_fAccShoot = 0.f;
	_float m_fShootTime = 0.1f;

private:
	void Use_Skill();
	void Shoot();
	void Find_Near_Target();


public:
	static CState_Enmu_Skill_1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const list<wstring>& AnimationList);
	virtual void Free() override;
};

END
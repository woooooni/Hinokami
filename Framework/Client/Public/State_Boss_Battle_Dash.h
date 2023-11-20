#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CState_Boss_Battle_Dash final : public CState
{
private:
	CState_Boss_Battle_Dash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine);
	virtual ~CState_Boss_Battle_Dash() = default;

public:
	HRESULT Initialize(const list<wstring>& AnimationList);

public:
	virtual void Enter_State(void* pArg = nullptr) override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CMonster* m_pBoss = nullptr;
	_bool m_bDoubleDash = false;

	_float m_fAccDashTime = 0.f;
	_float m_fDashCoolTime = 1.f;

public:
	static CState_Boss_Battle_Dash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const list<wstring>& AnimationList);
	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CState_Boss_Idle final : public CState
{
private:
	CState_Boss_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,class CStateMachine* pStateMachine);
	virtual ~CState_Boss_Idle() = default;

public:
	virtual HRESULT Initialize(const list<wstring>& AnimationList) override;

public:
	virtual void Enter_State(void* pArg = nullptr) override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


private:
	_float m_fDistance = 5.f;
	_float m_fAccAttackCoolTime = 0.f;
	_float m_fAttackCoolTime = 2.f;
	
	_bool m_bAttackable = false;

public:
	static CState_Boss_Idle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const list<wstring>& AnimationList);
	virtual void Free() override;
};

END
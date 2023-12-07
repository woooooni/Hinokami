#pragma once

#include "Client_Defines.h"
#include "State.h"
BEGIN(Engine)
class CModel;
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CState_Akaza_Attack_1 final : public CState
{

private:
	CState_Akaza_Attack_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine);
	virtual ~CState_Akaza_Attack_1() = default;

public:
	HRESULT Initialize(const list<wstring>& AnimationList);

public:
	virtual void Enter_State(void* pArg = nullptr) override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


private:
	class CMonster* m_pOwnerMonster = nullptr;
	class CGameObject* m_pTarget = nullptr;

private:
	_float m_fAttackEffect = .1f;
	_float m_fAccAttackEffect = 0.f;
	_bool m_bSound = false;

private:
	void Find_Near_Target();
	void Trace_Near_Target();

public:
	static CState_Akaza_Attack_1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const list<wstring>& AnimationList);
	virtual void Free() override;
};

END
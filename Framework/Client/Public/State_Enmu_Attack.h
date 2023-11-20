#pragma once

#include "Client_Defines.h"
#include "State.h"
BEGIN(Engine)
class CModel;
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CState_Enmu_Attack final : public CState
{

public:
	enum ENMU_ATTACK_TYPE { NEAR_ATTACK_0, NEAR_ATTACK_1, FAR_ATTACK_0, FAR_ATTACK_1, FAR_ATTACK_2, ATTACK_END };

private:
	CState_Enmu_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine);
	virtual ~CState_Enmu_Attack() = default;

public:
	HRESULT Initialize(const vector<list<wstring>>& AnimationList);

public:
	virtual void Enter_State(void* pArg = nullptr) override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


private:
	void Tick_Attack_0(_float fTimeDelta);
	void Tick_Attack_1(_float fTimeDelta);
	void Tick_Far_Attack_0(_float fTimeDelta);
	void Tick_Far_Attack_1(_float fTimeDelta);
	void Tick_Far_Attack_2(_float fTimeDelta);

private:
	vector<_uint> m_AnimationsIndex[ATTACK_END];
	_uint m_iRandomAttackIndex = 0;

	CMonster* m_pOwnerMonster = nullptr;

private:
	_bool Find_NearTarget();
	
public:
	static CState_Enmu_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const vector<list<wstring>>& AnimationList);
	virtual void Free() override;
};

END
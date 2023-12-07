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
	enum ENMU_ATTACK_TYPE { NEAR_ATTACK_0, FAR_ATTACK_0, FAR_ATTACK_1, ATTACK_END };

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
	void Tick_Near_Attack_0(_float fTimeDelta);
	void Tick_Far_Attack_0(_float fTimeDelta);
	void Tick_Far_Attack_1(_float fTimeDelta);

private:
	vector<_uint> m_AnimationsIndex[ENMU_ATTACK_TYPE::ATTACK_END];
	_uint m_iRandomAttackIndex = 0;
	class CMonster* m_pOwnerMonster = nullptr;

	class CGameObject* m_pTarget = nullptr;


private:
	void Shoot(class CGameObject* pTarget, _vector vOffsetPosition, _float fTimeDelta);
	class CGameObject* Find_NearTarget(_float fTimeDelta);
	_float Find_NearTarget_Distance(_float fTimeDelta);
	void Follow_NearTarget(_float fTimeDelta);
	void Trace_Near_Target();
	
public:
	static CState_Enmu_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const vector<list<wstring>>& AnimationList);
	virtual void Free() override;
};

END
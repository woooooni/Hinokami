#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CState_Akaza_Skill_0 final : public CState
{
private:
	CState_Akaza_Skill_0(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine);
	virtual ~CState_Akaza_Skill_0() = default;

public:
	virtual HRESULT Initialize(const list<wstring>& AnimationList) override;

public:
	virtual void Enter_State(void* pArg = nullptr) override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CBoss_Akaza* m_pOwnerBoss = nullptr;
	class CGameObject* m_pTarget = nullptr;


private:
	class CGameObject* m_pBoundaryObj = nullptr;

private:
	void Use_Skill();

	void Find_Near_Target();
	void Trace_Near_Target(_float fTimeDelta);


public:
	static CState_Akaza_Skill_0* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const list<wstring>& AnimationList);
	virtual void Free() override;
};

END
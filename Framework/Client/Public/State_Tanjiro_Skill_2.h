#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CState_Tanjiro_Skill_2 final : public CState
{
private:
	CState_Tanjiro_Skill_2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine);
	virtual ~CState_Tanjiro_Skill_2() = default;

public:
	HRESULT Initialize(const list<wstring>& AnimationList);

public:
	virtual void Enter_State(void* pArg = nullptr) override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


private:
	class CGameObject* m_pTarget = nullptr;
	class CCharacter* m_pCharacter = nullptr;
	class CSword* m_pSword = nullptr;

private:
	void Find_Near_Target();
	void Trace_Near_Target();
	void Use_Skill(_float fTimeDelta);

public:
	static CState_Tanjiro_Skill_2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const list<wstring>& AnimationList);
	virtual void Free() override;
};

END
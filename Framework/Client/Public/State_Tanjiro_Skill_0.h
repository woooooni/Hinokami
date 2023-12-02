#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CState_Tanjiro_Skill_0 final : public CState
{
private:
	CState_Tanjiro_Skill_0(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine);
	virtual ~CState_Tanjiro_Skill_0() = default;

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
	class CEffect* m_pSkillEffect = nullptr;

private:
	_float m_fAccSkillTime = 0.f;
	_float m_fSkillTime = 0.5f;

	_float m_fAccGenParticle = 0.f;
	_float m_fGenParticleTime = 0.1f;

private:
	void Find_Near_Target();
	void Use_Skill(_float fTimeDelta);

public:
	static CState_Tanjiro_Skill_0* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const list<wstring>& AnimationList);
	virtual void Free() override;
};

END
#include "stdafx.h"
#include "State_Enmu_Skill_0.h"
#include "GameInstance.h"
#include "Boss_Enmu.h"
#include "Model.h"
#include "Monster.h"
#include "Character.h"
#include "Animation.h"

CState_Enmu_Skill_0::CState_Enmu_Skill_0(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Enmu_Skill_0::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;

	m_pOwnerBoss = dynamic_cast<CBoss_Enmu*>(m_pOwner);
	if (nullptr == m_pOwnerBoss)
		return E_FAIL;

	return S_OK;
}

void CState_Enmu_Skill_0::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = 0;
	m_iCurrCount = 0;
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	m_pOwnerBoss->Set_Infinite(999.f, true);
	Use_Skill();
}

void CState_Enmu_Skill_0::Tick_State(_float fTimeDelta)
{
	switch (m_iCurrAnimIndex)
	{
	case 0:
		if (m_pModelCom->Get_CurrAnimation()->Is_Finished())		
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
		break;
	case 1:
		if (KEY_TAP(KEY::A) || KEY_TAP(KEY::D))
		{
			m_iCurrCount++;
			if (m_iCurrCount >= m_iKeyTabCount)
			{
				m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
				CStateMachine* pTargetMachine = m_pTarget->Get_Component<CStateMachine>(L"Com_StateMachine");
				pTargetMachine->Change_State(CCharacter::STATE::BATTLE_IDLE);
			}
		}
	}
}

void CState_Enmu_Skill_0::Exit_State()
{
	m_pOwnerBoss->Set_Infinite(0.f, false);
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

}

void CState_Enmu_Skill_0::Use_Skill()
{
	m_pTarget = nullptr;
	const list<CGameObject*>& Targets = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_MONSTER);
	for (auto& pTarget : Targets)
	{
		CTransform* pTargetTransform = pTarget->Get_Component<CTransform>(L"Com_Transform");

		if (nullptr == pTargetTransform)
			continue;

		m_pTarget = pTarget;
	}

	if (nullptr == m_pTarget)
	{
		m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
	}

	CStateMachine* pTargetMachine = m_pTarget->Get_Component<CStateMachine>(L"Com_StateMachine");
	pTargetMachine->Change_State(CCharacter::STATE::KNOCKDOWN);

}

CState_Enmu_Skill_0* CState_Enmu_Skill_0::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Enmu_Skill_0* pInstance = new CState_Enmu_Skill_0(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Enmu_Skill_0");
		return nullptr;
	}

	return pInstance;
}

void CState_Enmu_Skill_0::Free()
{
	__super::Free();
}

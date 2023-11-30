#include "stdafx.h"
#include "State_Enmu_Skill_0.h"
#include "GameInstance.h"
#include "Boss_Enmu.h"
#include "Model.h"
#include "Monster.h"

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
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pModelCom->Set_AnimIndex(m_AnimIndices[0]);
	m_pOwnerBoss->Set_Infinite(999.f, true);
}

void CState_Enmu_Skill_0::Tick_State(_float fTimeDelta)
{
	
}

void CState_Enmu_Skill_0::Exit_State()
{
	m_pOwnerBoss->Set_Infinite(0.f, false);
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

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

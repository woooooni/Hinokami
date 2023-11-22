#include "stdafx.h"
#include "State_Monster_Damaged_Blow.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"


CState_Monster_Damaged_Blow::CState_Monster_Damaged_Blow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Monster_Damaged_Blow::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;

	m_pOwnerMonster = dynamic_cast<CMonster*>(m_pOwner);
	if (nullptr == m_pOwnerMonster)
		return E_FAIL;

	return S_OK;
}

void CState_Monster_Damaged_Blow::Enter_State(void* pArg)
{
	m_pOwnerMonster->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pModelCom->Set_AnimIndex(m_AnimIndices[0]);

	m_fAccRecovery = 0.f;
}

void CState_Monster_Damaged_Blow::Tick_State(_float fTimeDelta)
{
	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[0]))
	{
		m_pModelCom->Set_AnimIndex(m_AnimIndices[1]);
	}

	if (m_pRigidBodyCom->Is_Ground() && 0.5f > XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pRigidBodyCom->Get_Velocity()))))
	{
		if (!m_bFirstGround)
		{
			m_bFirstGround = true;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[2]);
		}
		else
		{
			m_fAccRecovery += fTimeDelta;
			if (m_fAccRecovery >= m_fRecoveryTime)
			{
				m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
				m_fAccRecovery = 0.f;
			}
		}
	}
	
}

void CState_Monster_Damaged_Blow::Exit_State()
{
	m_pOwnerMonster->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_iCurrAnimIndex = 0;

	m_bFirstGround = false;
	m_fAccRecovery = 0.f;
}

CState_Monster_Damaged_Blow* CState_Monster_Damaged_Blow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Monster_Damaged_Blow* pInstance =  new CState_Monster_Damaged_Blow(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Character_Damaged_Bound");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Monster_Damaged_Blow::Free()
{
	__super::Free();
}

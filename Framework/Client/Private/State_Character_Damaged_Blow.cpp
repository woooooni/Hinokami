#include "stdafx.h"
#include "State_Character_Damaged_Blow.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"


CState_Character_Damaged_Blow::CState_Character_Damaged_Blow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Character_Damaged_Blow::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;
	

	m_pOwnerCharacter = dynamic_cast<CCharacter*>(m_pOwner);
	if (nullptr == m_pOwnerCharacter)
		return E_FAIL;

	return S_OK;
}

void CState_Character_Damaged_Blow::Enter_State(void* pArg)
{

	m_pOwnerCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pOwnerCharacter->Set_Infinite(999.f, true);
	m_pModelCom->Set_AnimIndex(m_AnimIndices[0]);
}

void CState_Character_Damaged_Blow::Tick_State(_float fTimeDelta)
{
	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[0]))
	{
		m_pModelCom->Set_AnimIndex(m_AnimIndices[1]);
	}

	if (0.5f > XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pRigidBodyCom->Get_Velocity()))))
	{
		if (!m_bFirstGround)
		{
			m_bFirstGround = true;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[2]);
		}

		m_fAccRecovery += fTimeDelta;
		if (m_fAccRecovery >= m_fRecoveryTime)
		{
			m_pStateMachineCom->Change_State(CCharacter::STATE::BATTLE_IDLE);
		}
	}
	
}

void CState_Character_Damaged_Blow::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_bFirstGround = false;
	m_fAccRecovery = 0.f;

	m_pOwnerCharacter->Set_Infinite(1.f, true);
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
}

CState_Character_Damaged_Blow* CState_Character_Damaged_Blow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Character_Damaged_Blow* pInstance =  new CState_Character_Damaged_Blow(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Character_Damaged_Bound");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Character_Damaged_Blow::Free()
{
	__super::Free();
	Safe_Release(m_pOwnerCharacter);
	Safe_Release(m_pRigidBodyCom);
}

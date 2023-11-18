#include "stdafx.h"
#include "State_Character_Damaged_Bound.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "RigidBody.h"


CState_Character_Damaged_Bound::CState_Character_Damaged_Bound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Character_Damaged_Bound::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;


	m_pOwnerCharacter = dynamic_cast<CCharacter*>(m_pOwner);
	if (nullptr == m_pOwnerCharacter)
		return E_FAIL;
	
	return S_OK;
}

void CState_Character_Damaged_Bound::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = 0;
	m_fAccRecovery = 0.f;

	m_pOwnerCharacter->Set_Infinite(999.f, true);

	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
}

void CState_Character_Damaged_Bound::Tick_State(_float fTimeDelta)
{

	if (m_pRigidBodyCom->Is_Ground())
	{
		if (!m_bFirstGround)
		{
			m_bFirstGround = true;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[1]);
		}
		else
		{
			m_fAccRecovery += fTimeDelta;
			if (m_fAccRecovery >= m_fRecoveryTime)
			{
				m_pStateMachineCom->Change_State(CCharacter::STATE::BATTLE_IDLE);
				m_fAccRecovery = 0.f;
			}
		}
		
	}
}

void CState_Character_Damaged_Bound::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_fAccRecovery = 0.f;
	m_bFirstGround = false;

	m_pOwnerCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pOwnerCharacter->Set_Infinite(1.f, true);
}

CState_Character_Damaged_Bound* CState_Character_Damaged_Bound::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Character_Damaged_Bound* pInstance =  new CState_Character_Damaged_Bound(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Character_Damaged_Bound");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Character_Damaged_Bound::Free()
{
	__super::Free();
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pOwnerCharacter);
}

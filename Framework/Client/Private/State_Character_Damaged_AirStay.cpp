#include "stdafx.h"
#include "State_Character_Damaged_AirStay.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "RigidBody.h"
#include "Character.h"
#include "Animation.h"

CState_Character_Damaged_AirStay::CState_Character_Damaged_AirStay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Character_Damaged_AirStay::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;


	m_pOwnerCharacter = dynamic_cast<CCharacter*>(m_pOwner);
	if (nullptr == m_pOwnerCharacter)
		return E_FAIL;
	
	return S_OK;
}

void CState_Character_Damaged_AirStay::Enter_State(void* pArg)
{
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

	m_iCurrAnimIndex = 0;
	m_fAccRecovery = 0.f;

	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	m_pRigidBodyCom->Set_Gravity(false);
}

void CState_Character_Damaged_AirStay::Tick_State(_float fTimeDelta)
{
	_float fProgress = m_pModelCom->Get_Animations()[m_AnimIndices[m_iCurrAnimIndex]]->Get_AnimationProgress();
	if (fProgress >= 0.7f && m_iCurrAnimIndex == 1)
	{
		m_pRigidBodyCom->Set_Gravity(true);
	}


	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[0]))
	{
		m_iCurrAnimIndex = 1;
		m_pModelCom->Set_AnimIndex(m_AnimIndices[1]);
	}

	if (m_pRigidBodyCom->Is_Ground())
	{
		if (!m_bFirstGround)
		{
			m_bFirstGround = true;
			m_iCurrAnimIndex = 2;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[2]);
		}
		else
		{
			m_fAccRecovery += fTimeDelta;
			if (m_fAccRecovery >= m_fRecoveryTime)
			{
				m_pStateMachineCom->Change_State(CMonster::IDLE);
				m_fAccRecovery = 0.f;
			}
				
		}
			
		
	}
}

void CState_Character_Damaged_AirStay::Exit_State()
{
	m_pOwnerCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

	m_iCurrAnimIndex = 0;
	m_fAccRecovery = 0.f;
	m_bFirstGround = false;

	m_pRigidBodyCom->Set_Gravity(true);
}

CState_Character_Damaged_AirStay* CState_Character_Damaged_AirStay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Character_Damaged_AirStay* pInstance =  new CState_Character_Damaged_AirStay(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Character_Damaged_AirStay");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Character_Damaged_AirStay::Free()
{
	__super::Free();
}

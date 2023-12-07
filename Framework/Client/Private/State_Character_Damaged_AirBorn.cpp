#include "stdafx.h"
#include "State_Character_Damaged_AirBorn.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "RigidBody.h"
#include "Animation.h"


CState_Character_Damaged_AirBorn::CState_Character_Damaged_AirBorn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Character_Damaged_AirBorn::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;

	m_pOwnerCharacter = dynamic_cast<CCharacter*>(m_pOwner);
	if (nullptr == m_pOwnerCharacter)
		return E_FAIL;
	
	return S_OK;
}

void CState_Character_Damaged_AirBorn::Enter_State(void* pArg)
{

	m_iCurrAnimIndex = 0;
	m_fAccRecovery = 0.f;

	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

}

void CState_Character_Damaged_AirBorn::Tick_State(_float fTimeDelta)
{
	_float fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();
	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[0]))
	{
		m_iCurrAnimIndex++;
		m_pModelCom->Set_AnimIndex(m_AnimIndices[1]);
	}

	if (m_iCurrAnimIndex == 1 && fProgress >= .8f)
	{
		m_pRigidBodyCom->Set_Gravity(true);
	}


	if (m_pRigidBodyCom->Is_Ground())
	{
		if (!m_bFirstGround)
		{
			m_bFirstGround = true;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[2]);
			m_pOwnerCharacter->Set_Infinite(999.f, true);
		}
		else
		{
			m_fAccRecovery += fTimeDelta;
			if (m_fAccRecovery >= m_fRecoveryTime)
			{
				m_fAccRecovery = 0.f;
				m_pOwnerCharacter->Set_Infinite(0.f, false);
				m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
			}
		}
	}
}

void CState_Character_Damaged_AirBorn::Exit_State()
{

	m_iCurrAnimIndex = 0;
	m_fAccRecovery = 0.f;
	m_bFirstGround = false;

	m_pOwnerCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
}

CState_Character_Damaged_AirBorn* CState_Character_Damaged_AirBorn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Character_Damaged_AirBorn* pInstance =  new CState_Character_Damaged_AirBorn(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Character_Damaged_AirBorn");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Character_Damaged_AirBorn::Free()
{
	__super::Free();
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pOwnerCharacter);
}

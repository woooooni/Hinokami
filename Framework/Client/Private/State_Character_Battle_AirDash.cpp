#include "stdafx.h"
#include "State_Character_Battle_AirDash.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "RigidBody.h"
#include "StateMachine.h"
#include "Sword.h"



CState_Character_Battle_AirDash::CState_Character_Battle_AirDash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Character_Battle_AirDash::Initialize(const list<wstring>& AnimationList)
{

	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;

	m_pCharacter = dynamic_cast<CCharacter*>(m_pStateMachineCom->Get_Owner());
	if (nullptr == m_pCharacter)
		return E_FAIL;

	m_pSword = m_pCharacter->Get_Part<CSword>(CCharacter::PART_SWORD);
	if (nullptr == m_pSword)
		return E_FAIL;
	return S_OK;
}

void CState_Character_Battle_AirDash::Enter_State(void* pArg)
{

	m_pCharacter->DrawSword();

	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::BODY, false);
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

	m_iCurrAnimIndex = 0;
	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	if (KEY_HOLD(KEY::A))
	{
		m_pRigidBodyCom->Add_Velocity(-1.f * XMVector3Normalize(vRight), 5.f);
		m_iCurrAnimIndex = 0;
	}
		
	else
	{
		m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(vRight), 5.f);
		m_iCurrAnimIndex = 1;
	}
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
		
}

void CState_Character_Battle_AirDash::Tick_State(_float fTimeDelta)
{
	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[m_iCurrAnimIndex]) || m_pRigidBodyCom->Is_Ground())
		m_pStateMachineCom->Change_State(CCharacter::STATE::BATTLE_IDLE);

}

void CState_Character_Battle_AirDash::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::BODY, true);
}

CState_Character_Battle_AirDash* CState_Character_Battle_AirDash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Character_Battle_AirDash* pInstance =  new CState_Character_Battle_AirDash(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Character_Battle_AirDash");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Character_Battle_AirDash::Free()
{
	__super::Free();
}

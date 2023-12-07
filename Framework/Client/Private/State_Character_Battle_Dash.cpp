#include "stdafx.h"
#include "State_Character_Battle_Dash.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "RigidBody.h"
#include "Sword.h"
#include "StateMachine.h"



CState_Character_Battle_Dash::CState_Character_Battle_Dash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Character_Battle_Dash::Initialize(const list<wstring>& AnimationList)
{
	if(FAILED(__super::Initialize(AnimationList)))
	return E_FAIL;

	m_pCharacter = dynamic_cast<CCharacter*>(m_pStateMachineCom->Get_Owner());
	if (nullptr == m_pCharacter)
		return E_FAIL;

	m_pSword = m_pCharacter->Get_Part<CSword>(CCharacter::PART_SWORD);
	if (nullptr == m_pSword)
		return E_FAIL;
	
	return S_OK;
}

void CState_Character_Battle_Dash::Enter_State(void* pArg)
{

	m_pCharacter->DrawSword();
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	
	_vector vRight  = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	if (KEY_HOLD(KEY::A))
	{
		m_iCurrAnimIndex = 0;
		m_pRigidBodyCom->Add_Velocity(-1.f * XMVector3Normalize(vRight), 5.f);
	}
	else
	{
		m_iCurrAnimIndex = 2;
		m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(vRight), 5.f);
	}

	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
}

void CState_Character_Battle_Dash::Tick_State(_float fTimeDelta)
{
	Input();
	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[m_iCurrAnimIndex]))
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
	}
		
}

void CState_Character_Battle_Dash::Exit_State()
{
	m_iCurrAnimIndex = 0;
}

void CState_Character_Battle_Dash::Input()
{
	if (m_iCurrAnimIndex == 1 || m_iCurrAnimIndex == 3)
		return;

	if (KEY_TAP(KEY::RBTN))
	{
		_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		if (KEY_HOLD(KEY::A))
		{
			m_iCurrAnimIndex = 1;
			m_pRigidBodyCom->Add_Velocity(-1.f * XMVector3Normalize(vRight), 10.f);
			
		}
		else
		{
			m_iCurrAnimIndex = 3;
			m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(vRight), 10.f);
		}
		m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	}
}

CState_Character_Battle_Dash* CState_Character_Battle_Dash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Character_Battle_Dash* pInstance =  new CState_Character_Battle_Dash(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Character_Battle_Dash");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Character_Battle_Dash::Free()
{
	__super::Free();
}

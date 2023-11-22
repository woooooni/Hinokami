#include "stdafx.h"
#include "State_Boss_Battle_Dash.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Animation.h"
#include "RigidBody.h"
#include "Sword.h"
#include "StateMachine.h"



CState_Boss_Battle_Dash::CState_Boss_Battle_Dash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Boss_Battle_Dash::Initialize(const list<wstring>& AnimationList)
{
	if(FAILED(__super::Initialize(AnimationList)))
	return E_FAIL;

	m_pBoss = dynamic_cast<CMonster*>(m_pStateMachineCom->Get_Owner());
	if (nullptr == m_pBoss)
		return E_FAIL;
	
	return S_OK;
}

void CState_Boss_Battle_Dash::Enter_State(void* pArg)
{
	m_pBoss->Set_ActiveColliders(CCollider::DETECTION_TYPE::BODY, false);
	m_pBoss->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_bDoubleDash = 5 > ((rand() + rand() + rand()) % 10);
	
	_vector vRight  = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));


	_uint iRandomDir = (rand() + rand() + rand()) % 2;

	if (iRandomDir == 0)
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

void CState_Boss_Battle_Dash::Tick_State(_float fTimeDelta)
{
	// Input();
	m_fAccDashTime += fTimeDelta;
	m_fDashCoolTime = 0.5f;
	if (m_fAccDashTime >= m_fDashCoolTime)
	{
		m_fAccDashTime = 0.f;
		if (m_bDoubleDash)
		{
			_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

			if (m_iCurrAnimIndex == 0)
				m_pRigidBodyCom->Add_Velocity(-1.f * XMVector3Normalize(vRight), 5.f);
			else
				m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(vRight), 5.f);

			m_iCurrAnimIndex++;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
			m_bDoubleDash = false;
		}
	}


	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[m_iCurrAnimIndex]))
	{
		m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
	}
		
}

void CState_Boss_Battle_Dash::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_bDoubleDash = false;
	m_fAccDashTime = 0.f;
	m_pBoss->Set_ActiveColliders(CCollider::DETECTION_TYPE::BODY, true);
}
//
//void CState_Character_Battle_Dash::Input()
//{
//	if (m_iCurrAnimIndex == 1 || m_iCurrAnimIndex == 3)
//		return;
//
//	if (KEY_TAP(KEY::RBTN))
//	{
//		_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
//		if (KEY_HOLD(KEY::A))
//		{
//			m_iCurrAnimIndex = 1;
//			m_pRigidBodyCom->Add_Velocity(-1.f * XMVector3Normalize(vRight), 10.f);
//			
//		}
//		else
//		{
//			m_iCurrAnimIndex = 3;
//			m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(vRight), 10.f);
//		}
//		m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
//	}
//}

CState_Boss_Battle_Dash* CState_Boss_Battle_Dash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Boss_Battle_Dash* pInstance =  new CState_Boss_Battle_Dash(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Boss_Battle_Dash");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Boss_Battle_Dash::Free()
{
	__super::Free();
}

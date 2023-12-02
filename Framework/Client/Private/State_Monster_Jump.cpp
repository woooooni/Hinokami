#include "stdafx.h"
#include "State_Monster_Jump.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "RigidBody.h"
#include "StateMachine.h"



CState_Monster_Jump::CState_Monster_Jump(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Monster_Jump::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;
	
	return S_OK;
}

void CState_Monster_Jump::Enter_State(void* pArg)
{
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	if (m_pRigidBodyCom == nullptr)
		

	if (nullptr == m_pRigidBodyCom)
		return;

	m_iCurrAnimIndex = 0;
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);


	_vector vJumpDir = XMVectorSet(XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), 1.f, XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), 0.f);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition = XMVectorSetY(vPosition, XMVectorGetY(vPosition) + 0.1f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(vJumpDir), 20.f);

	m_pRigidBodyCom->Set_Gravity(true);
	m_pRigidBodyCom->Set_Ground(false);
}

void CState_Monster_Jump::Tick_State(_float fTimeDelta)
{
	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[0]) || m_pModelCom->Is_Animation_Finished(m_AnimIndices[1]))
	{
		m_iCurrAnimIndex++;
		m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	}

	if (m_pRigidBodyCom->Is_Ground())
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
}

void CState_Monster_Jump::Exit_State()
{
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
}

CState_Monster_Jump* CState_Monster_Jump::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Monster_Jump* pInstance =  new CState_Monster_Jump(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Monster_Jump");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Monster_Jump::Free()
{
	__super::Free();
}

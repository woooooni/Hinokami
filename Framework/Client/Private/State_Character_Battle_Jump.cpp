#include "stdafx.h"
#include "State_Character_Battle_Jump.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "RigidBody.h"
#include "StateMachine.h"



CState_Character_Battle_Jump::CState_Character_Battle_Jump(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Character_Battle_Jump::Initialize(const list<wstring>& AnimationList)
{
	m_pModelCom = m_pStateMachineCom->Get_Owner()->Get_Component<CModel>(L"Com_Model");
	if (nullptr == m_pModelCom)
		return E_FAIL;


	m_pTransformCom = m_pStateMachineCom->Get_Owner()->Get_Component<CTransform>(L"Com_Transform");
	if (nullptr == m_pTransformCom)
		return E_FAIL;


	Safe_AddRef(m_pModelCom);
	Safe_AddRef(m_pTransformCom);

	for (auto strAnimName : AnimationList)
	{
		_int iAnimIndex = m_pModelCom->Find_AnimationIndex(strAnimName);
		if (-1 != iAnimIndex)
			m_AnimationIndices.push_back(iAnimIndex);
		else		
			return E_FAIL;
	}
	
	return S_OK;
}

void CState_Character_Battle_Jump::Enter_State(void* pArg)
{
	CGameObject* pOwner = m_pStateMachineCom->Get_Owner();
	if (nullptr != pOwner)
	{
		CCharacter* pCharacter = dynamic_cast<CCharacter*>(pOwner);
		if (pCharacter != nullptr)
			pCharacter->DrawSword();

		pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	}

	if (m_pRigidBody == nullptr)
		m_pRigidBody = m_pStateMachineCom->Get_Owner()->Get_Component<Engine::CRigidBody>(L"Com_RigidBody");

	if (nullptr == m_pRigidBody)
		return;

	m_iCurrAnimIndex = 0;
	m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iCurrAnimIndex]);


	_vector vJumpDir = XMVectorSet(XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), 1.f, XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), 0.f);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition = XMVectorSetY(vPosition, XMVectorGetY(vPosition) + 0.1f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pRigidBody->Add_Velocity(XMVector3Normalize(vJumpDir), 10.f);
}

void CState_Character_Battle_Jump::Tick_State(_float fTimeDelta)
{
	if (m_pModelCom->Is_Animation_Finished(m_AnimationIndices[0]) || m_pModelCom->Is_Animation_Finished(m_AnimationIndices[1]))
	{
		m_iCurrAnimIndex++;
		m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iCurrAnimIndex]);
	}


	// if(m_pRigidBody->)
	if (m_pRigidBody->Is_Ground())
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
}

void CState_Character_Battle_Jump::Exit_State()
{

}

CState_Character_Battle_Jump* CState_Character_Battle_Jump::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Character_Battle_Jump* pInstance =  new CState_Character_Battle_Jump(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Character_Battle_Jump");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Character_Battle_Jump::Free()
{
	__super::Free();
}

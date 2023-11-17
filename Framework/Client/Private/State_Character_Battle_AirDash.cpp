#include "stdafx.h"
#include "State_Character_Battle_AirDash.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "RigidBody.h"
#include "StateMachine.h"



CState_Character_Battle_AirDash::CState_Character_Battle_AirDash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Character_Battle_AirDash::Initialize(const list<wstring>& AnimationList)
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

void CState_Character_Battle_AirDash::Enter_State(void* pArg)
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
	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	if (KEY_HOLD(KEY::A))
	{
		m_pRigidBody->Add_Velocity(-1.f * XMVector3Normalize(vRight), 5.f);
		m_iCurrAnimIndex = 0;
	}
		
	else
	{
		m_pRigidBody->Add_Velocity(XMVector3Normalize(vRight), 5.f);
		m_iCurrAnimIndex = 1;
	}
	m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iCurrAnimIndex]);
		
}

void CState_Character_Battle_AirDash::Tick_State(_float fTimeDelta)
{
	if (m_pModelCom->Is_Animation_Finished(m_AnimationIndices[m_iCurrAnimIndex]) || m_pRigidBody->Is_Ground())
		m_pStateMachineCom->Change_State(CCharacter::STATE::BATTLE_IDLE);

}

void CState_Character_Battle_AirDash::Exit_State()
{
	m_iCurrAnimIndex = 0;
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

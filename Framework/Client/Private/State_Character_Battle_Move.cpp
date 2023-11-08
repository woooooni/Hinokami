#include "stdafx.h"
#include "State_Character_Battle_Move.h"
#include "GameInstance.h"
#include "Tanjiro.h"
#include "PipeLine.h"
#include "StateMachine.h"
#include "Navigation.h"

USING(Client)
CState_Character_Battle_Move::CState_Character_Battle_Move(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Character_Battle_Move::Initialize(const list<wstring>& AnimationList)
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

	m_pNavigation = m_pStateMachineCom->Get_Owner()->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr == m_pNavigation)
		return E_FAIL;
	
	Safe_AddRef(m_pNavigation);
	
	return S_OK;
}

void CState_Character_Battle_Move::Enter_State(void* pArg)
{
	CGameObject* pOwner = m_pStateMachineCom->Get_Owner();
	if (nullptr != pOwner)
	{
		CCharacter* pCharacter = dynamic_cast<CCharacter*>(pOwner);
		if (pCharacter != nullptr)
			pCharacter->DrawSword();

		pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	}

	m_iCurrAnimIndex = m_AnimationIndices[0];
	m_pModelCom->Set_AnimIndex(m_AnimationIndices[0]);
	m_fMoveSpeed = m_pTransformCom->Get_TickPerSecond();
}

void CState_Character_Battle_Move::Tick_State(_float fTimeDelta)
{
	_bool bKeyHolding = false;

	if (KEY_TAP(KEY::SHIFT))
	{
		m_iCurrAnimIndex = m_AnimationIndices[1];
		m_pModelCom->Set_AnimIndex(m_AnimationIndices[1]);
		m_pTransformCom->Set_TickPerSecond(m_fMoveSpeed + 10.f);
	}

	if (KEY_AWAY(KEY::SHIFT))
	{
		m_iCurrAnimIndex = m_AnimationIndices[0];
		m_pModelCom->Set_AnimIndex(m_AnimationIndices[0]);
		m_pTransformCom->Set_TickPerSecond(m_pTransformCom->Get_TickPerSecond() - 10.f);
	}
	

	if (KEY_HOLD(KEY::W))
	{
		bKeyHolding = true;


		_matrix vCamWolrd = GI->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);

		_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_vector vCamLook = vCamWolrd.r[CTransform::STATE_LOOK];

		vRight = XMVector3Normalize(vRight);
		vCamLook = XMVector3Normalize(vCamLook);

		_float fRadian = XMVectorGetX(XMVector3Dot(vRight, vCamLook)) * 10.f * fTimeDelta;


		m_pTransformCom->Rotation_Acc(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigation);
	}


	if (KEY_HOLD(KEY::S))
	{
		bKeyHolding = true;


		_matrix vCamWolrd = GI->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);

		_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_vector vCamLook = vCamWolrd.r[CTransform::STATE_LOOK];

		vRight = XMVector3Normalize(vRight);
		vCamLook = -1.f * XMVector3Normalize(vCamLook);

		_float fRadian = XMVectorGetX(XMVector3Dot(vRight, vCamLook)) * 10.f * fTimeDelta;


		m_pTransformCom->Rotation_Acc(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigation);
	}


	if (KEY_HOLD(KEY::A))
	{
		bKeyHolding = true;

		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 10.f, -1.f * fTimeDelta);
		// m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigation);
	}


	if (KEY_HOLD(KEY::D))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 10.f, fTimeDelta);
		// m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigation);
	}

	if (KEY_TAP(KEY::LBTN))
	{
		bKeyHolding = true;
		m_pStateMachineCom->Change_State(CCharacter::ATTACK);
	}


	if (KEY_TAP(KEY::SPACE))
	{
		bKeyHolding = true;
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_JUMP);
	}
		
		
		
	if (!bKeyHolding)
	{
		if (KEY_NONE(KEY::W) && KEY_NONE(KEY::A) && KEY_NONE(KEY::S) && KEY_NONE(KEY::D))		
			m_pStateMachineCom->Change_State(CTanjiro::BATTLE_IDLE);
	}
	
}

void CState_Character_Battle_Move::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_pTransformCom->Set_TickPerSecond(m_fMoveSpeed);
}

CState_Character_Battle_Move* CState_Character_Battle_Move::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Character_Battle_Move* pInstance =  new CState_Character_Battle_Move(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Character_Battle_Move");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Character_Battle_Move::Free()
{
	__super::Free();
	Safe_Release(m_pNavigation);
}

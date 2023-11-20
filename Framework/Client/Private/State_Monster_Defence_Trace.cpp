#include "stdafx.h"
#include "State_Monster_Defence_Trace.h"
#include "GameInstance.h"
#include "Monster.h"
#include "PipeLine.h"
#include "StateMachine.h"
#include "Navigation.h"
#include "GameObject.h"

USING(Client)
CState_Monster_Defence_Trace::CState_Monster_Defence_Trace(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Monster_Defence_Trace::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;
	
	return S_OK;
}

void CState_Monster_Defence_Trace::Enter_State(void* pArg)
{
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	
	m_iCurrAnimIndex = 0;
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	m_bAttackable = false;
	m_fAccAttackCoolTime = 0.f;
}

void CState_Monster_Defence_Trace::Tick_State(_float fTimeDelta)
{
	Find_NearTarget();
	m_fAccAttackCoolTime += fTimeDelta;
	if (m_fAccAttackCoolTime >= m_fAttackCoolTime)
	{
		m_fAccAttackCoolTime = 0.f;
		m_bAttackable = true;
	}

	if (m_pTarget == nullptr || m_pTarget->Is_ReserveDead() || m_pTarget->Is_Dead())
	{
		m_pStateMachineCom->Change_State(CMonster::IDLE);
		return;
	}
		

	CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>(L"Com_Transform");
	_vector vPosition = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f);
	_vector vTargetPosition = XMVectorSetY(pTargetTransform->Get_State(CTransform::STATE_POSITION), 0.f);

	_vector vDir = vTargetPosition - vPosition;
	XMVectorSetY(vDir, 0.f);

	if (XMVectorGetX(XMVector3Length(vDir)) < 1.2f)
	{
		if (m_bAttackable)
			m_pStateMachineCom->Change_State(CMonster::ATTACK);
		return;
	}
	else
	{
		m_pTransformCom->LookAt_ForLandObject(vTargetPosition);
		m_pTransformCom->Go_Straight(fTimeDelta, m_pStateMachineCom->Get_Owner()->Get_Component<CNavigation>(L"Com_Navigation"));
	}

}

void CState_Monster_Defence_Trace::Exit_State()
{
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pTarget = nullptr;
	m_iCurrAnimIndex = 0;
	m_bAttackable = false;
	m_fAccAttackCoolTime = 0.f;
}

void CState_Monster_Defence_Trace::Find_NearTarget()
{
	const list<CGameObject*>& Characters = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_CHARACTER);
	_float fDistance = 999999.f;
	for (auto& pCharacter : Characters)
	{
		CTransform* pTargetTransform = pCharacter->Get_Component<CTransform>(L"Com_Transform");

		if (pTargetTransform != nullptr)
		{
			_vector vTargetPos = m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
			_vector vPositon = pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION);

			_vector vDir = vTargetPos - vPositon;
			if (fDistance >= XMVectorGetX(XMVector3Length(vDir)))
			{
				fDistance = XMVectorGetX(XMVector3Length(vDir));
				m_pTarget = pCharacter;
			}
		}
	}

	const list<CGameObject*>&  Npcs = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_NPC);
	for (auto& pNpc : Npcs)
	{
		CTransform* pTargetTransform = pNpc->Get_Component<CTransform>(L"Com_Transform");

		if (pTargetTransform != nullptr)
		{
			_vector vTargetPos = m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
			_vector vPositon = pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION);

			_vector vDir = vTargetPos - vPositon;
			if (fDistance >= XMVectorGetX(XMVector3Length(vDir)))
			{
				fDistance = XMVectorGetX(XMVector3Length(vDir));
				m_pTarget = pNpc;
			}
		}
	}

}

CState_Monster_Defence_Trace* CState_Monster_Defence_Trace::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Monster_Defence_Trace* pInstance =  new CState_Monster_Defence_Trace(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Monster_Defence_Trace");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Monster_Defence_Trace::Free()
{
	__super::Free();
}

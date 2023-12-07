#include "stdafx.h"
#include "State_Akaza_Attack_0.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Animation.h"

CState_Akaza_Attack_0::CState_Akaza_Attack_0(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}


HRESULT CState_Akaza_Attack_0::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;



	m_pOwnerMonster = dynamic_cast<CMonster*>(m_pOwner);
	if (nullptr == m_pOwnerMonster)
		return E_FAIL;

	return S_OK;
}

void CState_Akaza_Attack_0::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = 0;
	m_pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 4.f, 1.f, true);

	m_pModelCom->Get_Animations()[m_AnimIndices[m_iCurrAnimIndex]]->Set_AnimationSpeed(2.f);
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);

}

void CState_Akaza_Attack_0::Tick_State(_float fTimeDelta)
{
	_float fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();
	switch (m_iCurrAnimIndex)
	{
	case 0:
		if (true == Follow_NearTarget(fTimeDelta))
		{
			m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 4.f, 1.f, false);
			m_pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);

			m_iCurrAnimIndex++;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
		}
		break;
	case 1:
		m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 4.f, 1.f, false);
		m_pOwnerMonster->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
		if (fProgress >= 0.9f)
		{
			m_iCurrAnimIndex++;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
			Find_Near_Target();
			Trace_Near_Target();
		}
		
		break;

	case 2:
		
		if (fProgress >= 0.9f)
		{
			m_iCurrAnimIndex++;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
			Find_Near_Target();
			Trace_Near_Target();
		}
		break;

	case 3:
		
		m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 4.f, 1.f, false);
		m_pOwnerMonster->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
		if (fProgress >= 0.9f)
		{
			m_iCurrAnimIndex++;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
			Find_Near_Target();
			Trace_Near_Target();
		}
		break;

	case 4:
		m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 4.f, 1.f, false);
		m_pOwnerMonster->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);

		if (fProgress >= 0.9f)
		{
			m_iCurrAnimIndex++;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
			m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BLOW, 0.f, 10.f, 1.f, false);
		}
		break;

	case 5:
		m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BLOW, 0.f, 10.f, 1.f, false);
		if (fProgress >= 0.9f)
		{
			m_pStateMachineCom->Change_State(CMonster::IDLE);
			return;
		}
		break;


	}


}

void CState_Akaza_Attack_0::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_pModelCom->Get_Animations()[m_AnimIndices[0]]->Set_AnimationSpeed(1.f);
	m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 0.f);
	m_pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
}




_bool CState_Akaza_Attack_0::Follow_NearTarget(_float fTimeDelta)
{
	m_pTarget = nullptr;

	list<CGameObject*>& TargetObjects = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_CHARACTER);
	for (auto& pTarget : TargetObjects)
	{
		CTransform* pTargetTransform = pTarget->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr == pTargetTransform)
			continue;
		else
			m_pTarget = pTarget;
	}

	if (nullptr != m_pTarget)
	{
		if (m_pTarget->Is_Dead() || m_pTarget->Is_ReserveDead())
		{
			m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
			return false;
		}

		CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr == pTargetTransform)
		{
			m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
			return false;
		}

		_vector vDir = XMVectorSetW(pTargetTransform->Get_Position() - m_pTransformCom->Get_Position(), 0.f);
		if (XMVectorGetX(XMVector3Length(vDir)) < 1.5f)
		{
			return true;
		}
		else
		{
			m_pTransformCom->LookAt_ForLandObject(pTargetTransform->Get_State(CTransform::STATE_POSITION));
			vDir = XMVector3Normalize(XMVectorSetY(vDir, 0.f));
			m_pTransformCom->Go_Dir(vDir, 15.f, fTimeDelta, m_pNavigationCom);
			return false;
		}
		
	}
	else
	{
		m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
		return false;
	}
}


void CState_Akaza_Attack_0::Find_Near_Target()
{
	_float fMinDistance = 9999999999.f;
	CTransform* pFindTargetTransform = nullptr;

	m_pTarget = nullptr;
	const list<CGameObject*>& Targets = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_CHARACTER);
	for (auto& pTarget : Targets)
	{
		CTransform* pTargetTransform = pTarget->Get_Component<CTransform>(L"Com_Transform");

		if (nullptr == pTargetTransform)
			continue;

		_float fDistance = XMVectorGetX(XMVector3Length(pTargetTransform->Get_Position() - m_pTransformCom->Get_Position()));
		if (fDistance < fMinDistance)
		{
			fMinDistance = fDistance;
			pFindTargetTransform = pTargetTransform;
			m_pTarget = pTarget;
			break;
		}
	}

	if (nullptr != pFindTargetTransform && fMinDistance < 20.f)
	{
		m_pTransformCom->LookAt_ForLandObject(pFindTargetTransform->Get_Position());
	}
}



void CState_Akaza_Attack_0::Trace_Near_Target()
{
	if (nullptr == m_pTarget)
	{
		m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_Look()), 6.f);
		return;
	}
	else
	{
		CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>(L"Com_Transform");

		if (nullptr == pTargetTransform)
		{
			m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_Look()), 6.f);
			return;
		}

		_float fDistance = XMVectorGetX(XMVector3Length(pTargetTransform->Get_Position() - m_pTransformCom->Get_Position()));
		if (fDistance <= 5.f)
		{
			m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_Look()), fDistance * 3.f);
		}
	}

}



CState_Akaza_Attack_0* CState_Akaza_Attack_0::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Akaza_Attack_0* pInstance = new CState_Akaza_Attack_0(pDevice, pContext, pStateMachine);

	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Akaza_Attack");
		return nullptr;
	}

	return pInstance;
}

void CState_Akaza_Attack_0::Free()
{
	__super::Free();
}

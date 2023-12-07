#include "stdafx.h"
#include "State_Akaza_Attack_1.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Animation.h"
#include "Effect_Manager.h"
#include "HierarchyNode.h"
#include "Utils.h"
#include "Particle_Manager.h"

CState_Akaza_Attack_1::CState_Akaza_Attack_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}


HRESULT CState_Akaza_Attack_1::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;

	m_pOwnerMonster = dynamic_cast<CMonster*>(m_pOwner);
	if (nullptr == m_pOwnerMonster)
		return E_FAIL;

	return S_OK;
}

void CState_Akaza_Attack_1::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = 0;

	m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 4.f, 1.f);
	m_pOwnerMonster->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	m_bSound = false;
	Find_Near_Target();
}

void CState_Akaza_Attack_1::Tick_State(_float fTimeDelta)
{
	_float fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();

	switch (m_iCurrAnimIndex)
	{
	case 0:
		// TODO EFFECT
		if (fProgress >= .6f)
		{
			Find_Near_Target();
			Trace_Near_Target();
			++m_iCurrAnimIndex;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
		}

		break;
	case 1:
		// TODO EFFECT
		m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 4.f, 1.f, false);
		m_pOwnerMonster->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
		if (fProgress >= .6f)
		{
			Find_Near_Target();
			Trace_Near_Target();
			++m_iCurrAnimIndex;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
		}
		break;
	case 2:
		// TODO EFFECT
		m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 4.f, 1.f, false);
		m_pOwnerMonster->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
		if (fProgress >= .7f)
		{
			Find_Near_Target();
			Trace_Near_Target();
			++m_iCurrAnimIndex;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
		}

		break;
	case 3:
		if (fProgress >= .8f)
		{
			Find_Near_Target();
			Trace_Near_Target();
			++m_iCurrAnimIndex;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
			m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 12.f, 3.f, 1.f);
			m_pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
		}
		break;
	case 4:
		if (fProgress >= .8f)
		{
			Find_Near_Target();
			Trace_Near_Target();
			++m_iCurrAnimIndex;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);

			m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 1.f, 1.f, false);
			m_pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);

			GI->Play_Sound(L"Voice_Akaza_Skill_0.wav", CHANNELID::SOUND_VOICE_MONSTER1, 1.f, true);
		}
		break;

	case 5:
		if (false == m_bSound)
		{
			GI->Play_Sound(L"Voice_Akaza_AttackCombo.wav", CHANNELID::SOUND_VOICE_MONSTER1, 1.f, true);
			m_bSound = true;
		}

		if (fProgress <= .7f)
		{
			m_fAccAttackEffect += fTimeDelta;
			m_fAttackEffect = .05f;
			if (m_fAccAttackEffect >= m_fAttackEffect)
			{
				m_fAccAttackEffect = 0.f;
				_matrix EffectWorldMatrix = m_pTransformCom->Get_WorldMatrix();
				_float fRandomX = CUtils::Random_Float(-1.f, 1.f);
				_float fRandomY = CUtils::Random_Float(-1.f, 1.f);
				_float fRandomZ = CUtils::Random_Float(-1.f, 1.f);
				EffectWorldMatrix.r[CTransform::STATE_POSITION] += EffectWorldMatrix.r[CTransform::STATE_LOOK] + XMVectorSet(0.f, 1.f, 0.f, 0.f) + XMVectorSet(fRandomX, fRandomY, fRandomZ, 0.f);

				CEffect_Manager::GetInstance()->Generate_Effect(L"Akaza_Attack_Wind_0", XMMatrixIdentity(), EffectWorldMatrix, 1.f);
				CEffect_Manager::GetInstance()->Generate_Effect(L"Fist_Wind_3", XMMatrixIdentity(), EffectWorldMatrix, 1.f);
				CParticle_Manager::GetInstance()->Generate_Particle(L"Akaza_Attack_Paritlce_1", EffectWorldMatrix);

				EffectWorldMatrix.r[CTransform::STATE_POSITION] -= EffectWorldMatrix.r[CTransform::STATE_LOOK] * .5f;
				CEffect_Manager::GetInstance()->Generate_Effect(L"Fist_0", XMMatrixIdentity(), EffectWorldMatrix, 1.f);
			}
		}
		if (fProgress >= .4f)
		{
			m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BLOW, 0.f, 8.f, 1.f, true);
			m_pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
		}
		if (fProgress >= .9f)
		{
			m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
		}
		break;
	}


}

void CState_Akaza_Attack_1::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_bSound = false;
	m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 0.f);
	m_pOwnerMonster->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
}

void CState_Akaza_Attack_1::Find_Near_Target()
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

void CState_Akaza_Attack_1::Trace_Near_Target()
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

CState_Akaza_Attack_1* CState_Akaza_Attack_1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Akaza_Attack_1* pInstance = new CState_Akaza_Attack_1(pDevice, pContext, pStateMachine);

	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Akaza_Attack_1");
		return nullptr;
	}

	return pInstance;
}

void CState_Akaza_Attack_1::Free()
{
	__super::Free();
}

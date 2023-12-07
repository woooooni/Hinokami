#include "stdafx.h"
#include "State_Akaza_Skill_0.h"
#include "GameInstance.h"
#include "Boss_Akaza.h"
#include "Model.h"
#include "Monster.h"
#include "Character.h"
#include "Animation.h"
#include "Effect_Manager.h"
#include "Akaza_Skill_Boundary.h"
#include "Camera_Manager.h"
#include "Particle_Manager.h"
#include "Utils.h"

CState_Akaza_Skill_0::CState_Akaza_Skill_0(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Akaza_Skill_0::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;

	m_pOwnerBoss = dynamic_cast<CBoss_Akaza*>(m_pOwner);
	if (nullptr == m_pOwnerBoss)
		return E_FAIL;


	return S_OK;
}

void CState_Akaza_Skill_0::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = 0;
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	m_pOwnerBoss->Set_Infinite(999.f, true);

	if (FAILED(GI->Add_GameObject(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Akaza_Skill_Boundary", nullptr, &m_pBoundaryObj)))
	{
		m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
		return;
	}


	_matrix OffsetMatrix = XMMatrixIdentity();
	OffsetMatrix.r[CTransform::STATE_POSITION] += XMVectorSet(0.f, 0.1f, 0.f, 0.f);

	CEffect_Manager::GetInstance()->Generate_Effect(L"Akaza_Skill_Snow", OffsetMatrix, m_pTransformCom->Get_WorldMatrix(), 2.f);
	GI->Play_Sound(L"Voice_Akaza_Skill_0.wav", CHANNELID::SOUND_VOICE_MONSTER1, 1.f, true);

}

void CState_Akaza_Skill_0::Tick_State(_float fTimeDelta)
{
	_float fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();
	switch (m_iCurrAnimIndex)
	{
	case 0:
		if (fProgress >= 0.75f)
		{
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
		}
		break;

	case 1:
		if (fProgress >= 0.75f)
		{
			CCamera* pCamera = CCamera_Manager::GetInstance()->Get_MainCamera();

			if (nullptr == pCamera)
				pCamera->Cam_Shake(1.f, 10.f);
				

			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
			Use_Skill();
		}
		break;

	case 2:
		Find_Near_Target();
		Trace_Near_Target(fTimeDelta);

		if (m_pRigidBodyCom->Is_Ground())
		{
			// ½ºÅ³ Äç!
			for (_uint i = 0; i < 5; ++i)
			{
				_matrix ParticleWorldMatrix = m_pTransformCom->Get_WorldMatrix();
				ParticleWorldMatrix.r[CTransform::STATE_POSITION] += XMVectorSet(CUtils::Random_Float(-5.f, 5.f), 0.f, CUtils::Random_Float(-5.f, 5.f), 0.f);
				CParticle_Manager::GetInstance()->Generate_Particle(L"Akaza_Attack_Paritlce_1", ParticleWorldMatrix);
				GI->Play_Sound(L"Akaza_Skill_0_Stone.wav", CHANNELID::SOUND_SKILL, 1.f, true);
			}

			CGameObject* pEffectStone = nullptr;

			GI->Add_GameObject(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Akaza_Stone_0", nullptr, &pEffectStone);
			CTransform* pTransform = pEffectStone->Get_Component<CTransform>(L"Com_Transform");
			_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
			pTransform->Set_WorldMatrix(WorldMatrix);
			pTransform->Set_Sclae(_float3(5.f, 5.f, 5.f));

			GI->Add_GameObject(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Akaza_Stone_1", nullptr, &pEffectStone);
			pTransform = pEffectStone->Get_Component<CTransform>(L"Com_Transform");
			pTransform->Set_WorldMatrix(WorldMatrix);
			pTransform->Set_Sclae(_float3(5.f, 5.f, 5.f));
			
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
			m_pBoundaryObj->Set_Dead(true);


			CCamera* pCamera = CCamera_Manager::GetInstance()->Get_MainCamera();
			if (nullptr != pCamera)
			{
				pCamera->Cam_Shake(1.f, 20.f);
			}
				
		}
		break;

	case 3:
		if (fProgress >= 0.9f)
		{
			m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
		}
		break;
	}
	
}

void CState_Akaza_Skill_0::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_pOwnerBoss->Set_Infinite(0.f, false);
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
}

void CState_Akaza_Skill_0::Use_Skill()
{
	Find_Near_Target();
	m_pRigidBodyCom->Set_Ground(false);

	m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + XMVectorSet(0.f, 200.f, 0.f, 0.f), GI->Get_TimeDelta(L"Timer_GamePlay"), m_pNavigationCom);
	
}

void CState_Akaza_Skill_0::Find_Near_Target()
{
	list<CGameObject*>& TargetObjects = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_CHARACTER);
	for (auto& pTarget : TargetObjects)
	{
		CTransform* pTargetTransform = pTarget->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr == pTargetTransform)
			continue;

		m_pTarget = pTarget;
		m_pTransformCom->LookAt_ForLandObject(pTargetTransform->Get_State(CTransform::STATE_POSITION));
	}
}

void CState_Akaza_Skill_0::Trace_Near_Target(_float fTimeDelta)
{
	CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>(L"Com_Transform");
	_vector vDir = pTargetTransform->Get_Position() - m_pTransformCom->Get_Position();
	vDir = XMVectorSetY(vDir, 0.f);

	if (XMVectorGetX(XMVector3Length(vDir)) > 0.1f)
	{
		m_pTransformCom->Go_Dir(XMVector3Normalize(vDir), 3.f, fTimeDelta, m_pNavigationCom);
	}
	

	CTransform* pObjTransform = m_pBoundaryObj->Get_Component<CTransform>(L"Com_Transform");
	_vector vPosition = XMVectorSetY(m_pTransformCom->Get_Position(), m_pRigidBodyCom->Get_RefHeight() + 0.5f);
	pObjTransform->Set_State(CTransform::STATE_POSITION, vPosition);

}

CState_Akaza_Skill_0* CState_Akaza_Skill_0::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Akaza_Skill_0* pInstance = new CState_Akaza_Skill_0(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Akaza_Skill_0");
		return nullptr;
	}

	return pInstance;
}

void CState_Akaza_Skill_0::Free()
{
	__super::Free();
}

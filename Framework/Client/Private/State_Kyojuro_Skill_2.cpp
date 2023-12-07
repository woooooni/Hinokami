#include "stdafx.h"
#include "State_Kyojuro_Skill_2.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "Sword.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"
#include "Monster.h"
#include "Camera_Manager.h"
#include "Camera.h"

CState_Kyojuro_Skill_2::CState_Kyojuro_Skill_2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Kyojuro_Skill_2::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;

	m_pCharacter = dynamic_cast<CCharacter*>(m_pStateMachineCom->Get_Owner());
	if (nullptr == m_pCharacter)
		return E_FAIL;

	m_pSword = m_pCharacter->Get_Part<CSword>(CCharacter::PART_SWORD);
	if (nullptr == m_pSword)
		return E_FAIL;

	return S_OK;
}

void CState_Kyojuro_Skill_2::Enter_State(void* pArg)
{
	m_fAccGenParticle = 0.f;
	m_iCurrAnimIndex = 0;
	m_bGenEffect = false;
	m_pCharacter->DrawSword();

	Use_Skill(GI->Get_TimeDelta(L"Timer_GamePlay"));
	GI->Play_Sound(L"Voice_Kyojuro_Skill_2_Use.wav", CHANNELID::SOUND_VOICE_CHARACTER, 1.f);
}

void CState_Kyojuro_Skill_2::Tick_State(_float fTimeDelta)
{
	
	_float fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();
	switch (m_iCurrAnimIndex)
	{
	case 0:
		m_fAccGenParticle += fTimeDelta;
		if (m_fAccGenParticle >= m_fGenParticleTime)
		{
			m_fAccGenParticle = 0.f;
			_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
			WorldMatrix.r[CTransform::STATE_POSITION] += XMVectorSet(0.f, 1.f, 0.f, 0.f);
			CParticle_Manager::GetInstance()->Generate_Particle(L"Kyojuro_Paritlcle_0", WorldMatrix);
			CParticle_Manager::GetInstance()->Generate_Particle(L"Kyojuro_Paritlcle_1", WorldMatrix);
		}

		if (fProgress >= .5f)
		{
			if (false == m_bGenEffect)
			{
				m_bGenEffect = true;
				_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
				WorldMatrix.r[CTransform::STATE_POSITION] += XMVectorSet(0.f, 1.f, 0.f, 0.f);
				CEffect_Manager::GetInstance()->Generate_Effect(L"Skl_02_Kyojuro_0", XMMatrixIdentity(), XMMatrixRotationX(-45.f) * WorldMatrix, 2.f);
				CEffect_Manager::GetInstance()->Generate_Effect(L"Skl_02_Kyojuro_1", XMMatrixIdentity(), XMMatrixRotationX(-45.f) * WorldMatrix, 2.f);

				CCamera* pCamera = CCamera_Manager::GetInstance()->Get_MainCamera();
				if (nullptr != pCamera)
				{
					pCamera->Cam_Shake(1.f, 3.f);
					GI->Play_Sound(L"Shake.wav", CHANNELID::SOUND_SHAKE, 1.f);
				}

				GI->Play_Sound(L"Kyojuro_Skill2_Use.wav", CHANNELID::SOUND_SKILL, 1.f);
			}
		}
		break;
	}

	if (fProgress >= .9f)
	{
		m_pStateMachineCom->Change_State(CCharacter::STATE::BATTLE_IDLE);
	}


}

void CState_Kyojuro_Skill_2::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_fAccGenParticle = 0.f;
	m_bGenEffect = false;

	m_pCharacter->SweathSword();
	m_pSword->Stop_Trail();

	m_pRigidBodyCom->Set_Friction_Scale(20.f);
	m_pCharacter->Set_Infinite(0.f, false);
	
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
	m_pCharacter->Set_ActiveColliders(CCollider::BODY, true);


	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);

	m_pRigidBodyCom->Set_Gravity(true);

}


void CState_Kyojuro_Skill_2::Find_Near_Target()
{
	m_pTarget = nullptr;
	_float fMinDistance = 9999999999.f;
	CTransform* pFindTargetTransform = nullptr;
	const list<CGameObject*>& Targets = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_MONSTER);
	for (auto& pTarget : Targets)
	{
		CTransform* pTargetTransform = pTarget->Get_Component<CTransform>(L"Com_Transform");

		if (nullptr == pTargetTransform)
			continue;

		_float fDistance = Vec3::Distance(pTargetTransform->Get_Position(), m_pTransformCom->Get_Position());
		if (fDistance < fMinDistance)
		{
			fMinDistance = fDistance;
			pFindTargetTransform = pTargetTransform;
			m_pTarget = pTarget;
		}
	}

	if (nullptr != pFindTargetTransform && fMinDistance < 20.f)
	{
		Vec4 vTargetPos = pFindTargetTransform->Get_Position();
		m_pTransformCom->LookAt_ForLandObject(vTargetPos);
	}
}


void CState_Kyojuro_Skill_2::Use_Skill(_float fTimeDelta)
{
	Find_Near_Target();
	m_iCurrAnimIndex = 0;
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	

	m_pCharacter->Set_Infinite(999.f, true);
	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 10.f, 0.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 10.f, 0.f, 1.f);

	m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
	m_pCharacter->Set_ActiveColliders(CCollider::BODY, true);

	m_pRigidBodyCom->Set_Friction_Scale(40.f);
	m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_Look()), 15.f);
}

CState_Kyojuro_Skill_2* CState_Kyojuro_Skill_2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Kyojuro_Skill_2* pInstance = new CState_Kyojuro_Skill_2(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Kyojuro_Skill_2");
		return nullptr;
	}

	return pInstance;
}

void CState_Kyojuro_Skill_2::Free()
{
	__super::Free();
}

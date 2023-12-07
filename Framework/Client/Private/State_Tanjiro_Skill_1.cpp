#include "stdafx.h"
#include "State_Tanjiro_Skill_1.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "Sword.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"
#include "Monster.h"

CState_Tanjiro_Skill_1::CState_Tanjiro_Skill_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Tanjiro_Skill_1::Initialize(const list<wstring>& AnimationList)
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

void CState_Tanjiro_Skill_1::Enter_State(void* pArg)
{
	m_fAccGenParticle = 0.f;
	m_iCurrAnimIndex = 0;
	m_pCharacter->DrawSword();

	Use_Skill(GI->Get_TimeDelta(L"Timer_GamePlay"));
	
	GI->Play_Sound(L"Zenitsu_Skill0_Ready.wav", CHANNELID::SOUND_SKILL, 1.f);
	GI->Play_Sound(L"Voice_Tanjiro_Skill_1_Use.wav", CHANNELID::SOUND_VOICE_CHARACTER, 1.f);
}

void CState_Tanjiro_Skill_1::Tick_State(_float fTimeDelta)
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
			CParticle_Manager::GetInstance()->Generate_Particle(L"Tanjiro_Paritlcle_0", WorldMatrix);
		}

		if (fProgress >= .6f)
		{
			m_pCharacter->Set_Infinite(0.f, false);
			m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);

			m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
			m_pCharacter->Set_ActiveColliders(CCollider::BODY, true);

			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
			m_pCharacter->Set_Infinite(0.f, false);
			m_pRigidBodyCom->Set_Friction_Scale(20.f);


			m_pSword->Stop_Trail();
		}
		break;
	case 1:
		if (fProgress >= 0.1f)
		{
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
		}
			
		break;

	case 2:
		if (fProgress >= 0.1f)
		{
			m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
		}
		break;
	}


}

void CState_Tanjiro_Skill_1::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_fAccGenParticle = 0.f;

	m_pCharacter->SweathSword();
	m_pSword->Stop_Trail();

	m_pRigidBodyCom->Set_Friction_Scale(20.f);
	m_pCharacter->Set_Infinite(0.f, false);
	m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, false);
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
	m_pCharacter->Set_ActiveColliders(CCollider::BODY, true);


	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);

	m_pRigidBodyCom->Set_Gravity(true);

}


void CState_Tanjiro_Skill_1::Find_Near_Target()
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


void CState_Tanjiro_Skill_1::Use_Skill(_float fTimeDelta)
{
	Find_Near_Target();
	m_iCurrAnimIndex = 0;
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	m_pSword->Generate_Trail(L"T_e_Skl_Wa_8Tak_Water011.png", L"T_e_Plc_P0003_Slash001.png", { 0.f, 0.f, 1.f, 1.f }, 66.f);
	

	m_pCharacter->Set_Infinite(999.f, true);
	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 4.f, 0.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 4.f, 0.f, 1.f);

	
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
	m_pCharacter->Set_ActiveColliders(CCollider::BODY, false);

	m_pRigidBodyCom->Set_Friction_Scale(40.f);
	m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_Look()), 15.f);
}

CState_Tanjiro_Skill_1* CState_Tanjiro_Skill_1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Tanjiro_Skill_1* pInstance = new CState_Tanjiro_Skill_1(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Tanjiro_Skill_1");
		return nullptr;
	}

	return pInstance;
}

void CState_Tanjiro_Skill_1::Free()
{
	__super::Free();
}

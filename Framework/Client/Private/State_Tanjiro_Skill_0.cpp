#include "stdafx.h"
#include "State_Tanjiro_Skill_0.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "Sword.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"
#include "Monster.h"

CState_Tanjiro_Skill_0::CState_Tanjiro_Skill_0(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Tanjiro_Skill_0::Initialize(const list<wstring>& AnimationList)
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

void CState_Tanjiro_Skill_0::Enter_State(void* pArg)
{
	
	m_iCurrAnimIndex = 0;
	m_fAccSkillTime = 0.f;
	m_fAccGenParticle = 0.f;

	Find_Near_Target();
	m_pCharacter->DrawSword();
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);

	GI->Play_Sound(L"Voice_Tanjiro_Skill_0_Use.wav", CHANNELID::SOUND_VOICE_CHARACTER, 1.f);
}

void CState_Tanjiro_Skill_0::Tick_State(_float fTimeDelta)
{
	
	_float fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();
	switch (m_iCurrAnimIndex)
	{
	case 0:
		if (fProgress >= .8f)
		{
			Use_Skill(fTimeDelta);
		}	
		break;

	case 1:
		m_fAccSkillTime += fTimeDelta;
		if (m_fAccSkillTime >= m_fSkillTime)
		{
			m_fAccSkillTime = 0.f;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);

			m_pSkillEffect->Set_Dead(true);
			Safe_Release(m_pSkillEffect);
			m_pSkillEffect = nullptr;
			

			m_pCharacter->Set_Infinite(0.f, false);
			m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
			m_pCharacter->Set_ActiveColliders(CCollider::BODY, true);


			m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
		}

		if (m_fAccGenParticle >= m_fGenParticleTime)
		{
			m_fAccGenParticle = 0.f;
			_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
			WorldMatrix.r[CTransform::STATE_POSITION] += XMVectorSet(0.f, 1.f, 0.f, 0.f);
			CParticle_Manager::GetInstance()->Generate_Particle(L"Tanjiro_Paritlcle_0", WorldMatrix);
		}

		m_pTransformCom->Go_Dir(XMVector3Normalize(m_pTransformCom->Get_Look()), 20.f, fTimeDelta, m_pNavigationCom);
		break;

	case 2:
		if (fProgress >= .8f)
		{
			m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
		}
		break;
	}


}

void CState_Tanjiro_Skill_0::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_fAccSkillTime = 0.f;
	m_fAccGenParticle = 0.f;

	m_pCharacter->SweathSword();
	m_pSword->Stop_Trail();


	m_pCharacter->Set_Infinite(0.f, false);
	m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, false);
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
	m_pCharacter->Set_ActiveColliders(CCollider::BODY, true);


	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);

	if (nullptr != m_pSkillEffect)
	{
		m_pSkillEffect->Set_Dead(true);
		Safe_Release(m_pSkillEffect);
		m_pSkillEffect = nullptr;
	}

}


void CState_Tanjiro_Skill_0::Find_Near_Target()
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


void CState_Tanjiro_Skill_0::Use_Skill(_float fTimeDelta)
{
	m_iCurrAnimIndex++;
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	Find_Near_Target();
	

	if (FAILED(CEffect_Manager::GetInstance()->Generate_Effect(L"Skl_01_Tanjiro_0", XMMatrixIdentity(), XMMatrixIdentity(), 999.f, m_pCharacter, &m_pSkillEffect)))
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
	}
	else
	{
		m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
		Safe_AddRef(m_pSkillEffect);
	}



	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_vector vPosition = WorldMatrix.r[CTransform::STATE_POSITION];
	WorldMatrix.r[CTransform::STATE_POSITION] += XMVectorSet(0.f, 1.f, 0.f, 0.f);

	WorldMatrix.r[CTransform::STATE_POSITION] = vPosition + m_pTransformCom->Get_Look() * .5f;
	CParticle_Manager::GetInstance()->Generate_Particle(L"Tanjiro_Paritlcle_0", WorldMatrix);


	WorldMatrix.r[CTransform::STATE_POSITION] = vPosition + m_pTransformCom->Get_Look() * 1.f;
	CParticle_Manager::GetInstance()->Generate_Particle(L"Tanjiro_Paritlcle_0", WorldMatrix);

	WorldMatrix.r[CTransform::STATE_POSITION] = vPosition + m_pTransformCom->Get_Look() * 2.f;
	CParticle_Manager::GetInstance()->Generate_Particle(L"Tanjiro_Paritlcle_0", WorldMatrix);



	m_pCharacter->Set_Infinite(999.f, true);
	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 9.f, 0.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 9.f, 0.f, 1.f);

	m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
	m_pCharacter->Set_ActiveColliders(CCollider::BODY, false);
}

CState_Tanjiro_Skill_0* CState_Tanjiro_Skill_0::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Tanjiro_Skill_0* pInstance = new CState_Tanjiro_Skill_0(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Tanjiro_Skill_0");
		return nullptr;
	}

	return pInstance;
}

void CState_Tanjiro_Skill_0::Free()
{
	__super::Free();
}

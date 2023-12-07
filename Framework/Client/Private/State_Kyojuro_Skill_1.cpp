#include "stdafx.h"
#include "State_Kyojuro_Skill_1.h"
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

CState_Kyojuro_Skill_1::CState_Kyojuro_Skill_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Kyojuro_Skill_1::Initialize(const list<wstring>& AnimationList)
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

void CState_Kyojuro_Skill_1::Enter_State(void* pArg)
{
	
	m_iCurrAnimIndex = 0;
	m_fAccSkillTime = 0.f;
	m_fAccGenParticle = 0.f;

	m_bGenProjectile = false;

	Find_Near_Target();
	m_pCharacter->DrawSword();
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);

	Use_Skill(GI->Get_TimeDelta(L"Timer_GamePlay"));

	GI->Play_Sound(L"Voice_Kyojuro_Skill_1_Use.wav", CHANNELID::SOUND_VOICE_CHARACTER, 1.f);
}

void CState_Kyojuro_Skill_1::Tick_State(_float fTimeDelta)
{
	
	_float fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();
	switch (m_iCurrAnimIndex)
	{
	case 0:
		if (fProgress >= 0.4f)
		{
			if (false == m_bGenProjectile)
			{
				m_bGenProjectile = true;
				CGameObject* pGameObject = nullptr;
				if (FAILED(GI->Add_GameObject(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Kyojuro_Projectile", nullptr, &pGameObject)))
					return;

				CCamera* pCamera = CCamera_Manager::GetInstance()->Get_MainCamera();
				if (nullptr != pCamera)
				{
					pCamera->Cam_Shake(1.f, 3.f);
					GI->Play_Sound(L"Shake.wav", CHANNELID::SOUND_SHAKE, 1.f);
				}
				
				GI->Play_Sound(L"Kyojuro_Skill1_Use.wav", CHANNELID::SOUND_SKILL, 1.f);
				CTransform* pProjectileTransform = pGameObject->Get_Component<CTransform>(L"Com_Transform");
				pProjectileTransform->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
				pProjectileTransform->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f));
			}
		}	
		break;
	}

	if (fProgress >= 0.8f)
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
	}


}

void CState_Kyojuro_Skill_1::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_fAccSkillTime = 0.f;
	m_fAccGenParticle = 0.f;

	m_pCharacter->SweathSword();
	m_pSword->Stop_Trail();


	m_pCharacter->Set_Infinite(0.f, false);
	m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, false);

	m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
	m_pCharacter->Set_ActiveColliders(CCollider::BODY, true);


	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);

	m_bGenProjectile = false;

}


void CState_Kyojuro_Skill_1::Find_Near_Target()
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


void CState_Kyojuro_Skill_1::Use_Skill(_float fTimeDelta)
{
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	Find_Near_Target();



	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_vector vPosition = WorldMatrix.r[CTransform::STATE_POSITION];
	WorldMatrix.r[CTransform::STATE_POSITION] += XMVectorSet(0.f, 1.f, 0.f, 0.f);
	WorldMatrix.r[CTransform::STATE_POSITION] = vPosition + m_pTransformCom->Get_Look() * .5f;


	m_pCharacter->Set_Infinite(999.f, true);
	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 9.f, 0.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 9.f, 0.f, 1.f);

	m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, false);
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
	m_pCharacter->Set_ActiveColliders(CCollider::BODY, false);

	
}

CState_Kyojuro_Skill_1* CState_Kyojuro_Skill_1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Kyojuro_Skill_1* pInstance = new CState_Kyojuro_Skill_1(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Kyojuro_Skill_1");
		return nullptr;
	}

	return pInstance;
}

void CState_Kyojuro_Skill_1::Free()
{
	__super::Free();
}

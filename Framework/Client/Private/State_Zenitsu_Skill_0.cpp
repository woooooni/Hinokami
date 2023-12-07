#include "stdafx.h"
#include "State_Zenitsu_Skill_0.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "Sweath.h"
#include "Sword.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"
#include "Camera_Manager.h"
#include "Monster.h"
#include "Camera.h"


CState_Zenitsu_Skill_0::CState_Zenitsu_Skill_0(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Zenitsu_Skill_0::Initialize(const list<wstring>& AnimationList)
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

void CState_Zenitsu_Skill_0::Enter_State(void* pArg)
{

	m_iCurrAnimIndex = 0;
	m_fAccGenParticle = 0.f;
	m_pCharacter->SweathSword();
	Find_Near_Target();

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[CTransform::STATE_POSITION] += m_pTransformCom->Get_Look() * .5f;
	if (FAILED(CEffect_Manager::GetInstance()->Generate_Effect(L"Zenitsu_Charging_0", XMMatrixIdentity(), WorldMatrix, 999.f, nullptr, &m_pChargingEffect)))
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
	}
	else
	{
		m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
		Safe_AddRef(m_pChargingEffect);
	}

	GI->Play_Sound(L"Voice_Zenitsu_Skill_0_Ready.wav", CHANNELID::SOUND_VOICE_CHARACTER, 1.f, true);
}

void CState_Zenitsu_Skill_0::Tick_State(_float fTimeDelta)
{
	
	_float fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();
	switch (m_iCurrAnimIndex)
	{
	case 0:
		Find_Near_Target();
		if (fProgress >= .8f)
		{
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
			CSweath* pSweath = m_pCharacter->Get_Part<CSweath>(CCharacter::PART_SWEATH);
			CTransform* pSweathTransform = pSweath->Get_Component<CTransform>(L"Com_Transform");
			_matrix SweathEffectMatrix = pSweathTransform->Get_WorldMatrix();

			CEffect_Manager::GetInstance()->Generate_Effect(L"Skl_01_Zenitsu_Sweath_0", XMMatrixIdentity(), SweathEffectMatrix, 1.f);
			CEffect_Manager::GetInstance()->Generate_Effect(L"Skl_01_Zenitsu_Sweath_1", XMMatrixIdentity(), SweathEffectMatrix, 1.f);

			GI->Play_Sound(L"Zenitsu_Skill0_Ready.wav", CHANNELID::SOUND_SKILL, 1.f, true);
		}
			
		break;

	case 1:
		Find_Near_Target();
		m_pCharacter->Set_Infinite(0.f, false);
		if (fProgress >= .5f)
			Input(fTimeDelta);
		break;

	case 2:
		Find_Near_Target();
		if (fProgress >= .8f)
		{
			Use_Skill(fTimeDelta);
		}
		break;

	case 3:
		m_fGenParticleTime = 0.05f;
		m_fAccGenParticle += fTimeDelta;
		if (m_fAccGenParticle >= m_fGenParticleTime)
		{
			m_fAccGenParticle = 0.f;
			_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
			WorldMatrix.r[CTransform::STATE_POSITION] += WorldMatrix.r[CTransform::STATE_LOOK];
			CParticle_Manager::GetInstance()->Generate_Particle(L"Skl_01_Zenitsu_Particle_0", WorldMatrix);
		}

		if (0.1f > XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pRigidBodyCom->Get_Velocity()))))
		{
			m_pCharacter->Set_Infinite(0.f, false);
			m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);

			m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
			m_pCharacter->Set_ActiveColliders(CCollider::BODY, true);

			m_pRigidBodyCom->Set_Friction_Scale(20.f);
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
		}

		
		break;
	case 4:
		if (fProgress >= .8f)
		{
			m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
		}
		break;
	}


}

void CState_Zenitsu_Skill_0::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_fAccGenParticle = 0.f;

	m_pCharacter->SweathSword();
	m_pSword->Stop_Trail();

	m_pCharacter->Set_Infinite(0.f, false);
	m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, false);
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
	m_pCharacter->Set_ActiveColliders(CCollider::BODY, true);


	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);

	m_pRigidBodyCom->Set_Gravity(true);

	if (nullptr != m_pChargingEffect)
	{
		m_pChargingEffect->Set_Dead(true);
		Safe_Release(m_pChargingEffect);
		m_pChargingEffect = nullptr;
	}

}


void CState_Zenitsu_Skill_0::Input(_float fTimeDelta)
{
	if (KEY_NONE(KEY::NUM_1))
	{
		Find_Near_Target();
		++m_iCurrAnimIndex;
	}
}

void CState_Zenitsu_Skill_0::Find_Near_Target()
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

	if (nullptr != pFindTargetTransform && fMinDistance < 40.f)
	{
		Vec4 vTargetPos = pFindTargetTransform->Get_Position();
		m_pTransformCom->LookAt_ForLandObject(vTargetPos);
	}
}


void CState_Zenitsu_Skill_0::Use_Skill(_float fTimeDelta)
{
	m_iCurrAnimIndex++;
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);

	if (nullptr != m_pChargingEffect)
	{
		m_pChargingEffect->Set_Dead(true);
		Safe_Release(m_pChargingEffect);
		m_pChargingEffect = nullptr;
	}
		
	Find_Near_Target();

	_matrix EffectMatrix = m_pTransformCom->Get_WorldMatrix();
	_vector vDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (nullptr != m_pTarget)
	{
		CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>(L"Com_Transform");
		_vector vTargetPosition = pTargetTransform->Get_Position();
		vDir = vTargetPosition - m_pTransformCom->Get_Position();
		_vector vCenterPosition = m_pTransformCom->Get_Position() + (vDir * .5f);

		EffectMatrix.r[CTransform::STATE_LOOK] = XMVector3Normalize(vDir);
		EffectMatrix.r[CTransform::STATE_RIGHT] = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVector3Normalize(EffectMatrix.r[CTransform::STATE_LOOK]));
		EffectMatrix.r[CTransform::STATE_UP] = XMVector3Cross(XMVector3Normalize(EffectMatrix.r[CTransform::STATE_LOOK]), XMVector3Normalize(EffectMatrix.r[CTransform::STATE_RIGHT]));
		EffectMatrix.r[CTransform::STATE_POSITION] = m_pTransformCom->Get_Position() + EffectMatrix.r[CTransform::STATE_LOOK] + XMVectorSet(0.f, 1.f, 0.f, 0.f);
	}
	else
	{
		vDir = XMVector3Normalize(m_pTransformCom->Get_Look());
		EffectMatrix.r[CTransform::STATE_LOOK] = XMVector3Normalize(vDir);
		EffectMatrix.r[CTransform::STATE_RIGHT] = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVector3Normalize(EffectMatrix.r[CTransform::STATE_LOOK]));
		EffectMatrix.r[CTransform::STATE_UP] = XMVector3Cross(XMVector3Normalize(EffectMatrix.r[CTransform::STATE_LOOK]), XMVector3Normalize(EffectMatrix.r[CTransform::STATE_RIGHT]));
		EffectMatrix.r[CTransform::STATE_POSITION] = m_pTransformCom->Get_Position() + EffectMatrix.r[CTransform::STATE_LOOK] + XMVectorSet(0.f, 1.f, 0.f, 0.f);
	}
	
	/*_matrix EffectMatrix = XMMatrixIdentity();
	EffectMatrix.r[CTransform::STATE_POSITION] = (m_pTransformCom->Get_Position() + XMVectorSet(0.f, 0.f, 0.f, 0.f)) + (XMVector3Normalize(vDir) * 1.f);*/

	_matrix SkillMatrix = XMMatrixIdentity();
	SkillMatrix.r[CTransform::STATE_POSITION] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	SkillMatrix.r[CTransform::STATE_POSITION] += SkillMatrix.r[CTransform::STATE_LOOK] * 1.5f;
	
	CEffect_Manager::GetInstance()->Generate_Effect(L"Skl_01_Zenitsu_0", SkillMatrix, XMMatrixIdentity(), .8f, m_pCharacter);
	CEffect_Manager::GetInstance()->Generate_Effect(L"Skl_01_Zenitsu_1", SkillMatrix, XMMatrixIdentity(), .8f, m_pCharacter);




	EffectMatrix.r[CTransform::STATE_POSITION] = (m_pTransformCom->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f)) + (-1.f * XMVector3Normalize(vDir));
	CEffect_Manager::GetInstance()->Generate_Effect(L"Skl_01_Zenitsu_Dash_0", XMMatrixIdentity(), EffectMatrix, 1.f);


	_matrix OffsetMatrix = XMMatrixIdentity();
	OffsetMatrix.r[CTransform::STATE_POSITION] = XMVectorSet(0.f, 1.f, 0.f, 1.f);
	CEffect_Manager::GetInstance()->Generate_Effect(L"Wind_0", OffsetMatrix, XMMatrixIdentity(), 1.f, m_pCharacter);


	

	

	m_pCharacter->Set_Infinite(999.f, true);
	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 9.f, 0.f, 1.f, false);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 9.f, 0.f, 1.f, false);

	
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
	m_pCharacter->Set_ActiveColliders(CCollider::BODY, false);

	m_pRigidBodyCom->Set_Friction_Scale(40.f);
	m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(vDir), 20.f);

	CCamera* pCamera = CCamera_Manager::GetInstance()->Get_MainCamera();
	if (nullptr != pCamera)
	{
		pCamera->Cam_Shake(.5f, 10.f);
	}

	GI->Play_Sound(L"Voice_Zenitsu_Skill_0_Use.wav", CHANNELID::SOUND_VOICE_CHARACTER, 1.f, true);
	GI->Play_Sound(L"Zenitsu_Skill0_Use.wav", CHANNELID::SOUND_SKILL, 1.f, true);
}

CState_Zenitsu_Skill_0* CState_Zenitsu_Skill_0::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Zenitsu_Skill_0* pInstance = new CState_Zenitsu_Skill_0(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Zenitsu_Skill_0");
		return nullptr;
	}

	return pInstance;
}

void CState_Zenitsu_Skill_0::Free()
{
	__super::Free();
}

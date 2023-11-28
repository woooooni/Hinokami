#include "stdafx.h"
#include "State_Zenitsu_Special.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "Sword.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"
#include "Monster.h"

CState_Zenitsu_Special::CState_Zenitsu_Special(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Zenitsu_Special::Initialize(const list<wstring>& AnimationList)
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

void CState_Zenitsu_Special::Enter_State(void* pArg)
{

	m_iCurrAnimIndex = 0;
	m_pCharacter->SweathSword();

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	if (FAILED(CEffect_Manager::GetInstance()->Generate_Effect(L"Zenitsu_Charging_0", XMMatrixIdentity(), XMMatrixIdentity(), 999.f, m_pOwner, &m_pChargingEffect)))
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
	}
	else
	{
		m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
		Safe_AddRef(m_pChargingEffect);
	}
}

void CState_Zenitsu_Special::Tick_State(_float fTimeDelta)
{
	
	_float fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();
	switch (m_iCurrAnimIndex)
	{
	case 0:
		if (fProgress >= .8f)
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
		break;

	case 1:
		m_pCharacter->Set_Infinite(0.f, false);
		if (fProgress >= 0.3f)
			Input(fTimeDelta);
		break;

	case 2:
		if (fProgress >= .8f)
		{
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
			Use_Skill(fTimeDelta);
		}
		break;

	case 3:
		if (fProgress >= .5f)
		{
			m_pCharacter->Set_Infinite(0.f, false);
			m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);

			m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
			m_pCharacter->Set_ActiveColliders(CCollider::BODY, true);
		}

		if (0.1f > XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pRigidBodyCom->Get_Velocity()))))
		{
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

void CState_Zenitsu_Special::Exit_State()
{
	m_iCurrAnimIndex = 0;

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


void CState_Zenitsu_Special::Input(_float fTimeDelta)
{
	if (KEY_NONE(KEY::NUM_1))
	{
		Find_Near_Target();
		++m_iCurrAnimIndex;
	}
}

void CState_Zenitsu_Special::Find_Near_Target()
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


void CState_Zenitsu_Special::Use_Skill(_float fTimeDelta)
{
	m_iCurrAnimIndex++;
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);

	if (nullptr != m_pChargingEffect)
	{
		m_pChargingEffect->Set_Dead(true);
		Safe_Release(m_pChargingEffect);
		m_pChargingEffect = nullptr;
	}
		

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
	


	CEffect_Manager::GetInstance()->Generate_Effect(L"Skl_01_Zenitsu_0", XMMatrixIdentity(), EffectMatrix, 1.f);
	CEffect_Manager::GetInstance()->Generate_Effect(L"Skl_01_Zenitsu_1", XMMatrixIdentity(), EffectMatrix, 1.f);




	EffectMatrix.r[CTransform::STATE_POSITION] = (m_pTransformCom->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f)) + (-1.f * XMVector3Normalize(vDir));
	CEffect_Manager::GetInstance()->Generate_Effect(L"Skl_01_Zenitsu_Dash_0", XMMatrixIdentity(), EffectMatrix, 1.f);


	_matrix OffsetMatrix = XMMatrixIdentity();
	OffsetMatrix.r[CTransform::STATE_POSITION] = XMVectorSet(0.f, 1.f, 0.f, 1.f);
	CEffect_Manager::GetInstance()->Generate_Effect(L"Wind_0", OffsetMatrix, XMMatrixIdentity(), 1.f, m_pCharacter);


	EffectMatrix = XMMatrixIdentity();
	EffectMatrix.r[CTransform::STATE_POSITION] = (m_pTransformCom->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f)) + (XMVector3Normalize(vDir) * 1.f);
	CParticle_Manager::GetInstance()->Generate_Particle(L"Skl_01_Zenitsu_Particle_0", EffectMatrix);
	CParticle_Manager::GetInstance()->Generate_Particle(L"Skl_01_Zenitsu_Particle_1", EffectMatrix);

	EffectMatrix.r[CTransform::STATE_POSITION] = (m_pTransformCom->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f)) + (XMVector3Normalize(vDir) * 2.f);
	CParticle_Manager::GetInstance()->Generate_Particle(L"Skl_01_Zenitsu_Particle_0", EffectMatrix);
	CParticle_Manager::GetInstance()->Generate_Particle(L"Skl_01_Zenitsu_Particle_1", EffectMatrix);


	m_pCharacter->Set_Infinite(999.f, true);
	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 4.f, 0.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 4.f, 0.f, 1.f);

	m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, true);
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
	m_pCharacter->Set_ActiveColliders(CCollider::BODY, false);

	m_pRigidBodyCom->Set_Friction_Scale(40.f);
	m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(vDir), 20.f);
}

CState_Zenitsu_Special* CState_Zenitsu_Special::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Zenitsu_Special* pInstance = new CState_Zenitsu_Special(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Zenitsu_Special");
		return nullptr;
	}

	return pInstance;
}

void CState_Zenitsu_Special::Free()
{
	__super::Free();
}

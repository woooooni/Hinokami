#include "stdafx.h"
#include "State_Zenitsu_Air_Attack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "Sword.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"
#include "Monster.h"

CState_Zenitsu_Air_Attack::CState_Zenitsu_Air_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Zenitsu_Air_Attack::Initialize(const list<wstring>& AnimationList)
{
	if(FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;

	m_pCharacter = dynamic_cast<CCharacter*>(m_pStateMachineCom->Get_Owner());
	if (nullptr == m_pCharacter)
		return E_FAIL;

	m_pSword = m_pCharacter->Get_Part<CSword>(CCharacter::PART_SWORD);
	if (nullptr == m_pSword)
		return E_FAIL;
	
	return S_OK;
}

void CState_Zenitsu_Air_Attack::Enter_State(void* pArg)
{

	m_iCurrAnimIndex = 0;
	m_pCharacter->DrawSword();
	

	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	m_pRigidBodyCom->Set_Velocity({ 0.f, 0.f, 0.f });
	m_pRigidBodyCom->Set_Gravity(false);

	Find_Near_Target();
	if (nullptr != m_pTarget)
	{
		Use_Skill(GI->Get_TimeDelta(L"Timer_GamePlay"));
	}
}

void CState_Zenitsu_Air_Attack::Tick_State(_float fTimeDelta)
{
	Input(fTimeDelta);
	_float fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();

	if (fProgress >= .8f && m_iCurrAnimIndex == 0)
	{
		m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
		m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, false);
		m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
		m_pCharacter->Set_ActiveColliders(CCollider::BODY, true);


		m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
		m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
	}
		
	
	if (1 == m_iCurrAnimIndex)
	{
		if (fProgress >= 0.99f)
		{
			if (m_pRigidBodyCom->Is_Ground())
			{
				m_pStateMachineCom->Change_State(CCharacter::STATE::BATTLE_IDLE);
			}
			m_pRigidBodyCom->Set_Gravity(true);
		}
			
	}
	//
	//if (fProgress >= 1.f)
	//{
	//	m_pStateMachineCom->Change_State(CCharacter::STATE::BATTLE_IDLE);
	//	return;
	//}
		
}

void CState_Zenitsu_Air_Attack::Exit_State()
{
	m_iCurrAnimIndex = 0;

	m_pCharacter->SweathSword();
	m_pSword->Stop_Trail();


	m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, false);
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
	m_pCharacter->Set_ActiveColliders(CCollider::BODY, true);
	

	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);

	m_pRigidBodyCom->Set_Gravity(true);

}


void CState_Zenitsu_Air_Attack::Input(_float fTimeDelta)
{
	if (KEY_TAP(KEY::RBTN))
	{
		Find_Near_Target();
		
		if (nullptr != m_pTarget)
		{
			m_iCurrAnimIndex = 0;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
			Use_Skill(fTimeDelta);
		}

	}


}

void CState_Zenitsu_Air_Attack::Find_Near_Target()
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


void CState_Zenitsu_Air_Attack::Use_Skill(_float fTimeDelta)
{
	CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>(L"Com_Transform");
	_vector vTargetPosition = pTargetTransform->Get_Position();
	_vector vDir = vTargetPosition - m_pTransformCom->Get_Position();
	_vector vCenterPosition = m_pTransformCom->Get_Position() + (vDir * .5f);

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[CTransform::STATE_LOOK] = XMVector3Normalize(vDir);
	WorldMatrix.r[CTransform::STATE_RIGHT] = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVector3Normalize(WorldMatrix.r[CTransform::STATE_LOOK]));
	WorldMatrix.r[CTransform::STATE_UP] = XMVector3Cross(XMVector3Normalize(WorldMatrix.r[CTransform::STATE_LOOK]), XMVector3Normalize(WorldMatrix.r[CTransform::STATE_RIGHT]));
	WorldMatrix.r[CTransform::STATE_POSITION] = m_pTransformCom->Get_Position() + WorldMatrix.r[CTransform::STATE_LOOK] + XMVectorSet(0.f, 1.f, 0.f, 0.f);


	CEffect_Manager::GetInstance()->Generate_Effect(L"Skl_01_Zenitsu_0", XMMatrixIdentity(), WorldMatrix, 1.f);
	CEffect_Manager::GetInstance()->Generate_Effect(L"Skl_01_Zenitsu_1", XMMatrixIdentity(), WorldMatrix, 1.f);




	WorldMatrix.r[CTransform::STATE_POSITION] = (m_pTransformCom->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f)) + (-1.f * XMVector3Normalize(vDir));
	CEffect_Manager::GetInstance()->Generate_Effect(L"Skl_01_Zenitsu_Dash_0", XMMatrixIdentity(), WorldMatrix, 1.f);


	//WorldMatrix.r[CTransform::STATE_POSITION] = m_pTransformCom->Get_Position() + (1.f * XMVector3Normalize(m_pTransformCom->Get_Look()) + XMVectorSet(0.f, 1.f, 0.f, 0.f));

	_matrix OffsetMatrix = XMMatrixIdentity();
	OffsetMatrix.r[CTransform::STATE_POSITION] = XMVectorSet(0.f, 1.f, 0.f, 1.f);
	CEffect_Manager::GetInstance()->Generate_Effect(L"Wind_0", OffsetMatrix, XMMatrixIdentity(), 1.f, m_pCharacter);



	WorldMatrix = XMMatrixIdentity();
	WorldMatrix.r[CTransform::STATE_POSITION] = (m_pTransformCom->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f)) + (XMVector3Normalize(vDir) * 1.f);
	CParticle_Manager::GetInstance()->Generate_Particle(L"Skl_01_Zenitsu_Particle_0", WorldMatrix);
	CParticle_Manager::GetInstance()->Generate_Particle(L"Skl_01_Zenitsu_Particle_1", WorldMatrix);

	WorldMatrix.r[CTransform::STATE_POSITION] = (m_pTransformCom->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f)) + (XMVector3Normalize(vDir) * 2.f);
	CParticle_Manager::GetInstance()->Generate_Particle(L"Skl_01_Zenitsu_Particle_0", WorldMatrix);
	CParticle_Manager::GetInstance()->Generate_Particle(L"Skl_01_Zenitsu_Particle_1", WorldMatrix);
	

	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 4.f, 0.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 4.f, 0.f, 1.f);

	m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, true);
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
	m_pCharacter->Set_ActiveColliders(CCollider::BODY, false);

	m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(vDir), 10.f);
}

CState_Zenitsu_Air_Attack* CState_Zenitsu_Air_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Zenitsu_Air_Attack* pInstance =  new CState_Zenitsu_Air_Attack(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Zenitsu_Air_Attack");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Zenitsu_Air_Attack::Free()
{
	__super::Free();
}

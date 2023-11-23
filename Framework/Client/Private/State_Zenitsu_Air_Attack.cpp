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
	m_iSkillCount = 3;
	m_iCurrAnimIndex = 0;
	m_pCharacter->DrawSword();
	

	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	Follow_Near_Target(GI->Get_TimeDelta(L"Timer_GamePlay"));

	m_pRigidBodyCom->Set_Velocity({ 0.f, 0.f, 0.f });
	m_pRigidBodyCom->Set_Gravity(false);

	m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, true);
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);

	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 4.f, 0.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 4.f, 0.f, 1.f);
}

void CState_Zenitsu_Air_Attack::Tick_State(_float fTimeDelta)
{
	_float fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();

	if(fProgress >= .8f)
		Input(fTimeDelta);
	else
	{
		if (0 == m_iCurrAnimIndex && fProgress >= .2f)
			Input(fTimeDelta);

	}

	if (fProgress >= .9f)
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
		return;
	}



	switch (m_iCurrAnimIndex)
	{
	case 0:
		break;
	case 1:
		if (fProgress >= .5f)
		{
			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
		}
		else
		{
			m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 0.f, 4.f, 1.f);
			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 0.f, 4.f, 1.f);

			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
		}
		break;
	case 2:
		if (fProgress >= .5f)
		{
			

			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
		}
		else
		{
			if (fProgress >= 4.f)
			{
				m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BLOW, 0.f, 4.f, 1.f);
				m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BLOW, 0.f, 4.f, 1.f);
				m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
			}
			
		}
		break;
	}
}

void CState_Zenitsu_Air_Attack::Exit_State()
{
	m_iCurrAnimIndex = 0;

	m_pCharacter->SweathSword();
	m_pSword->Stop_Trail();


	m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, false);
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);

	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);

	m_pRigidBodyCom->Set_Gravity(true);

}


void CState_Zenitsu_Air_Attack::Input(_float fTimeDelta)
{
	if (KEY_TAP(KEY::LBTN))
	{
		switch (m_iCurrAnimIndex)
		{
		case 0:
			Find_Near_Target();
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
			m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 4.f, 0.f, 1.f);
			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 4.f, 0.f, 1.f);
			break;
		case 1:
			Find_Near_Target();
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
			m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 4.f, 0.f, 1.f);
			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 4.f, 0.f, 1.f);
			break;
		}
	}


}

void CState_Zenitsu_Air_Attack::Find_Near_Target()
{
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
		}
	}

	if (nullptr != pFindTargetTransform && fMinDistance < 20.f)
	{
		Vec4 vTargetPos = pFindTargetTransform->Get_Position();
		m_pTransformCom->LookAt_ForLandObject(vTargetPos);
	}
}


void CState_Zenitsu_Air_Attack::Follow_Near_Target(_float fTimeDelta)
{
	list<CGameObject*>& Monsters = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_MONSTER);
	_float fDistance = 99999999999.f;

	CGameObject* pTarget = nullptr;
	_vector vNearTargetPosition;
	for (auto& pMonster : Monsters)
	{
		if (nullptr == pMonster || pMonster->Is_ReserveDead() || pMonster->Is_Dead())
			continue;

		CStateMachine* pMachine = pMonster->Get_Component<CStateMachine>(L"Com_StateMachine");
		if (nullptr == pMachine)
			continue;

		if (pMachine->Get_CurrState() != CMonster::MONSTER_STATE::DAMAGED_AIRBORN)
			continue;

		CTransform* pTransform = pMonster->Get_Component<CTransform>(L"Com_Transform");
		if (pTransform == nullptr)
			continue;

		_vector vTargetPosition = pTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_vector vDir = vTargetPosition - vPosition;

		_float fLen = XMVectorGetX(XMVector3Length(vDir));
		if (fLen < fDistance)
		{
			fDistance = fLen;
			pTarget = pMonster;
			vNearTargetPosition = vTargetPosition;
		}
	}


	if (pTarget != nullptr && fDistance <= 10.f)
	{
		
		CTransform* pTargetTransform = pTarget->Get_Component<CTransform>(L"Com_Transform");
		
		_vector vTargetLook = pTargetTransform->Get_State(CTransform::STATE_LOOK);
		_vector vTargetPosition =  pTargetTransform->Get_State(CTransform::STATE_POSITION);

		_vector vFollowPosition = (XMVectorSetY(vTargetPosition, XMVectorGetY(vTargetPosition))) + (XMVector3Normalize(vTargetLook) * 0.1f);

		m_pTransformCom->Set_Position(XMVectorSetW(vFollowPosition, 1.f), fTimeDelta, m_pCharacter->Get_Component<CNavigation>(L"Com_Navigation"));
		m_pTransformCom->LookAt_ForLandObject(vTargetPosition);		
	}

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
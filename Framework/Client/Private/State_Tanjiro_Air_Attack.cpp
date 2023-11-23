#include "stdafx.h"
#include "State_Tanjiro_Air_Attack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "Sword.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"
#include "Monster.h"

CState_Tanjiro_Air_Attack::CState_Tanjiro_Air_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Tanjiro_Air_Attack::Initialize(const list<wstring>& AnimationList)
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

void CState_Tanjiro_Air_Attack::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = 0;
	m_pCharacter->DrawSword();
	
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	Follow_Near_Target(GI->Get_TimeDelta(L"Timer_GamePlay"));
}

void CState_Tanjiro_Air_Attack::Tick_State(_float fTimeDelta)
{
	Input(fTimeDelta);
	_float fProgress = m_pModelCom->Get_Animations()[m_AnimIndices[m_iCurrAnimIndex]]->Get_AnimationProgress();

	if (m_iCurrAnimIndex == 0)
	{
		if (m_pRigidBodyCom->Is_Ground())
		{
			m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
		}
	}
	else if (m_iCurrAnimIndex == 1)
	{
		if (fProgress >= 0.3f && fProgress <= 0.5f)
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
		else
		{
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
		}
			

		if (fProgress >= 0.8f)
		{
			m_pRigidBodyCom->Set_Gravity(true);
		}
			

	}
	else if (m_iCurrAnimIndex == 2)
	{
		if (fProgress >= 0.3f && fProgress <= 0.5f)		
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
		else		
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

		if (fProgress >= 0.8f)
		{
			m_pRigidBodyCom->Set_Gravity(true);
		}
	}
	
	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[m_iCurrAnimIndex]) && m_pRigidBodyCom->Is_Ground())
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
	}
}

void CState_Tanjiro_Air_Attack::Exit_State()
{
	m_iCurrAnimIndex = 0;

	m_pSword->Stop_Trail();
	m_pSword->Set_ActiveColliders(CCollider::BODY, true);
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);

	m_pRigidBodyCom->Set_Gravity(true);
	m_pRigidBodyCom->Set_Ground(false);

	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
}


void CState_Tanjiro_Air_Attack::Input(_float fTimeDelta)
{
	_float fLookVelocity = 3.f;
	_float fAnimationProgress = m_pModelCom->Get_Animations()[m_AnimIndices[m_iCurrAnimIndex]]->Get_AnimationProgress();
	if (fAnimationProgress >= 0.5f)
	{
		if (KEY_TAP(KEY::LBTN))
		{
			m_iCurrAnimIndex = min(m_iCurrAnimIndex + 1, m_AnimIndices.size());
			if (m_iCurrAnimIndex == m_AnimIndices.size())
			{
				m_iCurrAnimIndex -= 1;
				return;
			}

			if (m_iCurrAnimIndex != m_AnimIndices.size())
				m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);

			
			switch (m_iCurrAnimIndex)
			{
			case 1:
				Follow_Near_Target(fTimeDelta);
				m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 5.f, 0.f, 1.f);
				m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
				m_pRigidBodyCom->Set_Gravity(false);
				m_pRigidBodyCom->Set_Ground(false);
				break;

			case 2:
				// Follow_Near_Target();
				m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
				m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BLOW, 0.f, 5.f, 1.f);
				m_pRigidBodyCom->Set_Gravity(false);
				m_pRigidBodyCom->Set_Ground(false);
				break;

			}
		}
	}
}

void CState_Tanjiro_Air_Attack::Follow_Near_Target(_float fTimeDelta)
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
		m_pTransformCom->LookAt_ForLandObject(vNearTargetPosition);
		CTransform* pTargetTransform = pTarget->Get_Component<CTransform>(L"Com_Transform");
		
		_vector vTargetLook = pTargetTransform->Get_State(CTransform::STATE_LOOK);
		_vector vTargetPosition =  pTargetTransform->Get_State(CTransform::STATE_POSITION);

		_vector vFollowPosition = XMVectorSetY(XMVector3Normalize(vTargetLook), 0.f)  + XMVectorSetY(vTargetPosition, XMVectorGetY(vTargetPosition) - 0.5f);

		m_pTransformCom->Set_Position(XMVectorSetW(vFollowPosition, 1.f), GI->Get_TimeDelta(L"Timer_GamePlay"), m_pCharacter->Get_Component<CNavigation>(L"Com_Navigation"));
	}

}

CState_Tanjiro_Air_Attack* CState_Tanjiro_Air_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Tanjiro_Air_Attack* pInstance =  new CState_Tanjiro_Air_Attack(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Tanjiro_Air_Attack");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Tanjiro_Air_Attack::Free()
{
	__super::Free();
}

#include "stdafx.h"
#include "State_Tanjiro_Attack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "Sword.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"

CState_Tanjiro_Attack::CState_Tanjiro_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Tanjiro_Attack::Initialize(const list<wstring>& AnimationList)
{
	m_pModelCom = m_pStateMachineCom->Get_Owner()->Get_Component<CModel>(L"Com_Model");
	if (nullptr == m_pModelCom)
		return E_FAIL;


	m_pTransformCom = m_pStateMachineCom->Get_Owner()->Get_Component<CTransform>(L"Com_Transform");
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_pRigidBodyCom = m_pStateMachineCom->Get_Owner()->Get_Component<CRigidBody>(L"Com_RigidBody");
	if (nullptr == m_pRigidBodyCom)
		return E_FAIL;

	m_pCharacter = dynamic_cast<CCharacter*>(m_pStateMachineCom->Get_Owner());
	if (nullptr == m_pCharacter)
		return E_FAIL;

	m_pSword = m_pCharacter->Get_Part<CSword>(CCharacter::PART_SWORD);
	if (nullptr == m_pSword)
		return E_FAIL;

	Safe_AddRef(m_pRigidBodyCom);
	Safe_AddRef(m_pCharacter);
	Safe_AddRef(m_pModelCom);
	Safe_AddRef(m_pTransformCom);


	for (auto strAnimName : AnimationList)
	{
		_int iAnimIndex = m_pModelCom->Find_AnimationIndex(strAnimName);
		if (-1 != iAnimIndex)
			m_AnimationIndices.push_back(iAnimIndex);
		else		
			return E_FAIL;
	}
	
	return S_OK;
}

void CState_Tanjiro_Attack::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = 0;

	Find_Near_Target();
	m_pCharacter->DrawSword();
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);

	m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iCurrAnimIndex]);
	/*if (FAILED(CEffect_Manager::GetInstance()->Generate_Effect(L"Slash_0", XMMatrixIdentity(), m_pSword->Get_FinalWorldMatrix(), 1.f, m_pSword)))
		return;*/

	m_pSword->Stop_Trail();


}

void CState_Tanjiro_Attack::Tick_State(_float fTimeDelta)
{
	
	Input(fTimeDelta);
	
	_float fProgress = m_pModelCom->Get_Animations()[m_AnimationIndices[m_iCurrAnimIndex]]->Get_AnimationProgress();

	if (m_pModelCom->Is_Animation_Finished(m_AnimationIndices[m_iCurrAnimIndex]))
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
	}
	
	switch (m_iCurrAnimIndex)
	{
	case 0:
		if (fProgress > 0.5f)
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
		m_pSword->Set_Collider_AttackMode(CCollider::DETECTION_TYPE::ATTACK, CCollider::ATTACK_TYPE::BASIC);
		break;

	case 1:
		m_pSword->Set_Collider_AttackMode(CCollider::DETECTION_TYPE::ATTACK, CCollider::ATTACK_TYPE::BASIC);
		break;

	case 2:
		m_pSword->Set_Collider_AttackMode(CCollider::DETECTION_TYPE::ATTACK, CCollider::ATTACK_TYPE::BASIC);
		break;

	case 3:
		if (fProgress >= 0.2f && fProgress <= 0.6f)
		{
			if (fProgress <= 0.4f)
				m_pSword->Set_Collider_AttackMode(CCollider::DETECTION_TYPE::ATTACK, CCollider::ATTACK_TYPE::AIR_BORN);
			else
				m_pSword->Set_Collider_AttackMode(CCollider::DETECTION_TYPE::ATTACK, CCollider::ATTACK_TYPE::BOUND);

			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
		}
		else
		{
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
		}
		break;

	case 4:
		if (fProgress >= 0.2f && fProgress <= 0.6f)
		{
			m_pSword->Set_Collider_AttackMode(CCollider::DETECTION_TYPE::ATTACK, CCollider::ATTACK_TYPE::BLOW);
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
		}
		else
		{
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
		}
			
		break;
	}

}

void CState_Tanjiro_Attack::Exit_State()
{
	m_iCurrAnimIndex = 0;
	
	Find_Near_Target();

	// m_pSword->Stop_Trail();
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
	m_pSword->Set_Collider_AttackMode(CCollider::DETECTION_TYPE::ATTACK, CCollider::ATTACK_TYPE::BASIC);
}


void CState_Tanjiro_Attack::Input(_float fTimeDelta)
{
	_float fLookVelocity = 3.f;
	_float fAnimationProgress = m_pModelCom->Get_Animations()[m_AnimationIndices[m_iCurrAnimIndex]]->Get_AnimationProgress();
	if (fAnimationProgress >= 0.3f)
	{
		if (KEY_TAP(KEY::LBTN))
		{
			m_iCurrAnimIndex = min(m_iCurrAnimIndex + 1, m_AnimationIndices.size());
			if (m_iCurrAnimIndex == m_AnimationIndices.size())
			{
				m_iCurrAnimIndex -= 1;
				return;
			}

				

			if (m_iCurrAnimIndex != m_AnimationIndices.size())
				m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iCurrAnimIndex]);

			
			switch (m_iCurrAnimIndex)
			{
			case 1:
				Find_Near_Target();
				m_pSword->Set_Damage(1.f);
				m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), fLookVelocity);
				break;

			case 2:
				Find_Near_Target();
				m_pSword->Set_Damage(1.f);
				m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), fLookVelocity);
				break;

			case 3:
				Find_Near_Target();
				m_pSword->Set_Damage(3.f);
				m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), fLookVelocity + 5.f);
				break;

			case 4:
				Find_Near_Target();
				m_pSword->Set_Damage(3.f);
				m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), fLookVelocity);
				m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
				break;

			default:
				break;
			}
		}
	}
}

void CState_Tanjiro_Attack::Find_Near_Target()
{
	list<CGameObject*> Monsters = GI->Find_GameObjects(LEVEL_GAMEPLAY, LAYER_TYPE::LAYER_MONSTER);
	_float fDistance = 99999999999.f;

	CGameObject* pTarget = nullptr;
	_vector vNearTargetPosition;
	for (auto& pMonster : Monsters)
	{
		if (nullptr == pMonster || pMonster->Is_ReserveDead() || pMonster->Is_Dead())
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
	}
}

CState_Tanjiro_Attack* CState_Tanjiro_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Tanjiro_Attack* pInstance =  new CState_Tanjiro_Attack(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Tanjiro_Attack");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Tanjiro_Attack::Free()
{
	__super::Free();
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pCharacter);

}

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

void CState_Tanjiro_Attack::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = 0;

	Find_Near_Target();
	m_pCharacter->DrawSword();
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);

	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);

	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);

	m_pSword->SetUp_Trail_Position(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, -1.5f, 1.f));
	m_pSword->Stop_Trail();
	m_pSword->Generate_Trail(L"T_e_Skl_In_Slash_Line003.png", L"T_e_cmn_Slash006_Reverse.png", _float4(0.561f, 0.945f, 1.f, 1.f), 44);

}

void CState_Tanjiro_Attack::Tick_State(_float fTimeDelta)
{
	
	Input(fTimeDelta);
	
	_float fProgress = m_pModelCom->Get_Animations()[m_AnimIndices[m_iCurrAnimIndex]]->Get_AnimationProgress();

	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[m_iCurrAnimIndex]))
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
	}
	
	switch (m_iCurrAnimIndex)
	{
	case 0:
		if (fProgress > 0.5f)
		{
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Stop_Trail();
		}
			
		m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
		break;

	case 1:
		if (fProgress > 0.5f)
		{
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Stop_Trail();
		}

		m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
		break;

	case 2:
		if (fProgress > 0.5f)
		{
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Stop_Trail();
		}
		m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
		break;

	case 3:
		if (fProgress >= 0.2f && fProgress <= 0.6f)
		{
			if (fProgress <= 0.4f)
				m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 7.f, 0.1f, 1.f);
			else			
				m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BOUND, -5.f, 1.5f, 1.f);
				

			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
		}
		else
		{
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
		}
		break;

	case 4:
		if (fProgress >= 0.2f && fProgress <= 0.55f)
		{
			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BLOW, 0.f, 5.f, 1.f);
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
		}
		else
		{
			if (fProgress > 0.55f)
			{
				m_pSword->Stop_Trail();
			}
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
		}
			
		break;
	}

}

void CState_Tanjiro_Attack::Exit_State()
{
	m_iCurrAnimIndex = 0;
	
	Find_Near_Target();

	m_pSword->Stop_Trail();
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
}


void CState_Tanjiro_Attack::Input(_float fTimeDelta)
{
	_float fLookVelocity = 4.f;

	_float fAnimationProgress = m_pModelCom->Get_Animations()[m_AnimIndices[m_iCurrAnimIndex]]->Get_AnimationProgress();
	if (fAnimationProgress >= 0.3f)
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
				Find_Near_Target();
				m_pSword->Generate_Trail(L"T_e_Skl_In_Slash_Line003.png", L"T_e_cmn_Slash006_Reverse.png", _float4(0.561f, 0.945f, 1.f, 1.f), 44);
				m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
				m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), 0.f);
				break;

			case 2:
				Find_Near_Target();
				m_pSword->Generate_Trail(L"T_e_Skl_In_Slash_Line003.png", L"T_e_cmn_Slash006_Reverse.png", _float4(0.561f, 0.945f, 1.f, 1.f), 66);
				m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
				m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), fLookVelocity + 2.f);
				break;

			case 3:
				Find_Near_Target();
				m_pSword->Generate_Trail(L"T_e_Skl_In_Slash_Line003.png", L"T_e_cmn_Slash006_Reverse.png", _float4(0.561f, 0.945f, 1.f, 1.f), 44);
				m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
				m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), fLookVelocity + 5.f);
				break;

			case 4:
				Find_Near_Target();
				m_pSword->Generate_Trail(L"T_e_Skl_In_Slash_Line003.png", L"T_e_cmn_Slash006_Reverse.png", _float4(0.561f, 0.945f, 1.f, 1.f), 44);
				m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), fLookVelocity);
				m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
				break;

			default:
				break;
			}
		}

		if (KEY_TAP(KEY::RBTN))
		{
			if (m_iCurrAnimIndex == 3)			
				m_pStateMachineCom->Change_State(CCharacter::AIR_ATTACK);
		}
	}
}

void CState_Tanjiro_Attack::Find_Near_Target()
{
	list<CGameObject*>& Monsters = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_MONSTER);
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
}

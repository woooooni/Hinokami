#include "stdafx.h"
#include "State_Zenitsu_Attack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "Sword.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"

CState_Zenitsu_Attack::CState_Zenitsu_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Zenitsu_Attack::Initialize(const list<wstring>& AnimationList)
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

void CState_Zenitsu_Attack::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = 0;

	Find_Near_Target();
	Trace_Near_Target();

	m_pCharacter->SweathSword();

	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);

	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 4.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 4.f, 1.f);
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);


	
}

void CState_Zenitsu_Attack::Tick_State(_float fTimeDelta)
{
	_float fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();

	if (fProgress >= .8f)
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
		return;
	}
	else if (fProgress >= .3f)
	{
		Input(fTimeDelta);
	}



	switch (m_iCurrAnimIndex)
	{
	case 0:
		break;
	case 1:
		m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
		m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
		if (fProgress >= 0.5f)
		{
			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
		}
		else
		{
			if (fProgress >= .15f && fProgress <= .16f)
			{
				CEffect_Manager::GetInstance()->Generate_Effect(L"Slash_0", XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixRotationY(XMConvertToRadians(-270.f)), XMMatrixIdentity(), 2.f, m_pSword);
			}
			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
		}
		break;
	case 2:
		if (fProgress >= 0.5f)
		{
			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
		}
		else
		{
			if (fProgress >= 0.1f && fProgress < 0.11f)
			{
				_vector vPosition  = m_pTransformCom->Get_Position();
				_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
				WorldMatrix.r[CTransform::STATE_POSITION] = XMVectorSetY(vPosition, XMVectorGetY(vPosition) + 1.f);

				CEffect_Manager::GetInstance()->Generate_Effect(L"Zenitsu_Slash_0", XMMatrixRotationZ(XMConvertToRadians(30.f)), WorldMatrix, 1.f);
				CEffect_Manager::GetInstance()->Generate_Effect(L"Zenitsu_Slash_1", XMMatrixRotationZ(XMConvertToRadians(150.f)), WorldMatrix, 1.f);
			}

			if (fProgress >= .2f && fProgress <= .21f)
			{
				_vector vPosition = m_pTransformCom->Get_Position();
				_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
				WorldMatrix.r[CTransform::STATE_POSITION] = XMVectorSetY(vPosition, XMVectorGetY(vPosition) + .5f);

				CEffect_Manager::GetInstance()->Generate_Effect(L"Zenitsu_Slash_0", XMMatrixRotationZ(XMConvertToRadians(-15.f)), WorldMatrix, 1.f);
				CEffect_Manager::GetInstance()->Generate_Effect(L"Zenitsu_Slash_1", XMMatrixRotationZ(XMConvertToRadians(-165.f)), WorldMatrix, 1.f);
			}

			if (fProgress >= .4f && fProgress <= .41f)
			{
				_vector vPosition = m_pTransformCom->Get_Position();
				_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
				WorldMatrix.r[CTransform::STATE_POSITION] = XMVectorSetY(vPosition, XMVectorGetY(vPosition) + .5f);

				CEffect_Manager::GetInstance()->Generate_Effect(L"Zenitsu_Slash_0", XMMatrixRotationZ(XMConvertToRadians(60.f)), WorldMatrix, 1.f);
				CEffect_Manager::GetInstance()->Generate_Effect(L"Zenitsu_Slash_1", XMMatrixRotationZ(XMConvertToRadians(120.f)), WorldMatrix, 1.f);
			}

			m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
		}
		break;
	case 3:
		if (fProgress >= 0.2f && fProgress <= 0.3f)
		{
			Vec3 vLook = m_pTransformCom->Get_Look();

			XMVectorSetY(vLook, 0.f);
			vLook.Normalize(vLook);

			m_pRigidBodyCom->Add_Velocity(vLook, 5.f);

			m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 7.f, 0.f, 1.f);
			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);

			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 7.f, 0.f, 1.f);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
		}
		else if (fProgress >= 0.5f)
		{
			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
			m_pSword->Set_ActiveColliders(CCollider::BASIC, true);

			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
		}
		break;
	case 4:
		if (fProgress >= 0.1f && fProgress < 0.4f)
		{
			if (fProgress >= 0.18f && fProgress <= 0.19f)
			{
				CEffect_Manager::GetInstance()->Generate_Effect(L"Slash_0", XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixRotationY(XMConvertToRadians(-270.f)), XMMatrixIdentity(), 2.f, m_pSword);
			}

			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BLOW, 0.f, 7.f, 1.f);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
		}
		else
		{
			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);

		}

		/*if (fProgress >= 0.2f && fProgress <= 0.3f)
		{
			Vec3 vLook = m_pTransformCom->Get_Look();

			XMVectorSetY(vLook, 0.f);
			vLook.Normalize(vLook);

			m_pRigidBodyCom->Add_Velocity(vLook, 5.f);

			m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 7.f, 0.f, 1.f);
			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);

			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 7.f, 0.f, 1.f);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
		}
		else if (fProgress >= 0.5f)
		{
			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
			m_pSword->Set_ActiveColliders(CCollider::BASIC, true);

			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
		}*/
		break;
	//case 4:
	//	if (fProgress >= 0.1f && fProgress < 0.4f)
	//	{
	//		m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BLOW, 0.f, 10.f, 1.f);
	//		m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
	//	}
	//	else if(fProgress >= 0.4f)
	//	{
	//		m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	//		m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
	//		
	//	}
	//	break;
	}


}

void CState_Zenitsu_Attack::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_pCharacter->SweathSword();

	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);

	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
}


void CState_Zenitsu_Attack::Input(_float fTimeDelta)
{
	if (KEY_HOLD(KEY::RBTN))
	{
		switch (m_iCurrAnimIndex)
		{
		case 2:
			Find_Near_Target();
			Trace_Near_Target();
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
			break;
		case 3:
			m_pStateMachineCom->Change_State(CCharacter::AIR_ATTACK);
			break;
		}
	}

	if (KEY_HOLD(KEY::LBTN))
	{
		switch (m_iCurrAnimIndex)
		{
		case 0:
			Find_Near_Target();
			Trace_Near_Target();
			m_pCharacter->DrawSword();
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
			
			break;
		case 1:
			Find_Near_Target();
			Trace_Near_Target();
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
			
			break;
		case 2:
			Find_Near_Target();
			Trace_Near_Target();
			m_iCurrAnimIndex = 4;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
			break;
		//case 3:
		//	Find_Near_Target();
		//	Trace_Near_Target();
		//	m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
		//	break;
		}

		return;
	}
}

void CState_Zenitsu_Attack::Find_Near_Target()
{
	_float fMinDistance = 9999999999.f;
	CTransform* pFindTargetTransform = nullptr;

	m_pTarget = nullptr;
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

void CState_Zenitsu_Attack::Trace_Near_Target()
{
	if (nullptr == m_pTarget)
	{
		m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_Look()), 4.f);
		return;
	}
	else
	{
		CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>(L"Com_Transform");

		if (nullptr == pTargetTransform)
		{
			m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_Look()), 4.f);
			return;
		}

		_float fDistance = Vec3::Distance(pTargetTransform->Get_Position(), m_pTransformCom->Get_Position());
		if (fDistance <= 5.f)
		{
			m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_Look()), fDistance * 3.f);
		}
		
	}

}

CState_Zenitsu_Attack* CState_Zenitsu_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Zenitsu_Attack* pInstance =  new CState_Zenitsu_Attack(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Zenitsu_Attack");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Zenitsu_Attack::Free()
{
	__super::Free();
}

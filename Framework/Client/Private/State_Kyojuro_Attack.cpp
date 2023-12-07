#include "stdafx.h"
#include "State_Kyojuro_Attack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "Sword.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"
#include "Utils.h"

CState_Kyojuro_Attack::CState_Kyojuro_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Kyojuro_Attack::Initialize(const list<wstring>& AnimationList)
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

void CState_Kyojuro_Attack::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = 0;

	Find_Near_Target();

	m_pCharacter->DrawSword();

	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);

	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 4.f, 1.f, false);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 4.f, 1.f, false);
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);

	for (_uint i = 0; i < 8; ++i)
	{
		m_bSlashEffect[i] = false;
	}

}

void CState_Kyojuro_Attack::Tick_State(_float fTimeDelta)
{
	_float fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();

	if (fProgress >= .98f)
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
		return;
	}
	else if (fProgress >= .3f)
	{
		Input(fTimeDelta);
	}


	fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();
	switch (m_iCurrAnimIndex)
	{
	case 0:
		if (false == m_bSlashEffect[m_iCurrAnimIndex])
		{
			m_bSlashEffect[m_iCurrAnimIndex] = true;
			_matrix WorldMatrix = XMMatrixRotationZ(XMConvertToRadians(110.f)) * m_pTransformCom->Get_WorldMatrix();
			WorldMatrix.r[CTransform::STATE_POSITION] = m_pTransformCom->Get_Position() + XMVectorSet(0.f, .5f, 0.f, 0.f);
			CEffect_Manager::GetInstance()->Generate_Effect(L"Slash_0", XMMatrixIdentity(), WorldMatrix, 2.f);

			GI->Play_Sound(L"Slash_0.wav", CHANNELID::SOUND_SLASH, 0.3f, true);

		}
		break;
	case 1:
		m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 3.f, false);
		m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 3.f, false);

		if (fProgress >= 0.5f)
		{
			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
		}
		break;
	case 2:
		if (fProgress >= 0.4f)
		{
			if (false == m_bSlashEffect[3])
			{
				m_bSlashEffect[3] = true;
				_matrix WorldMatrix = XMMatrixRotationZ(XMConvertToRadians(-45.f)) * m_pTransformCom->Get_WorldMatrix();
				WorldMatrix.r[CTransform::STATE_POSITION] = m_pTransformCom->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f);
				CEffect_Manager::GetInstance()->Generate_Effect(L"Slash_0", XMMatrixIdentity(), WorldMatrix, 2.f);

				GI->Play_Sound(L"Slash_0.wav", CHANNELID::SOUND_SLASH, 0.3f, true);
			}
		}

		if (fProgress >= 0.7f)
		{
			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
		}
		else
		{
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f, false);
		}
		break;
	case 3:
		if (fProgress >= 0.7f)
		{
			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
		}

		if (fProgress >= 0.4f)
		{
			if (false == m_bSlashEffect[4])
			{
				m_bSlashEffect[4] = true;
				_matrix WorldMatrix = XMMatrixRotationZ(XMConvertToRadians(-80.f)) * m_pTransformCom->Get_WorldMatrix();
				WorldMatrix.r[CTransform::STATE_POSITION] = m_pTransformCom->Get_Position() + XMVectorSet(0.f, 1.5f, 0.f, 0.f);
				CEffect_Manager::GetInstance()->Generate_Effect(L"Slash_0", XMMatrixIdentity(), WorldMatrix, 2.f);

				GI->Play_Sound(L"Slash_0.wav", CHANNELID::SOUND_SLASH, 0.3f, true);
			}
		}

		m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
		m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f, false);
		break;

	case 4:
		if (fProgress >= 0.4f && fProgress <= 0.5f)
		{
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 12.f, 1.f, 1.f, true);
			
			if (false == m_bSlashEffect[5])
			{
				m_bSlashEffect[5] = true;
				_matrix WorldMatrix = XMMatrixRotationZ(XMConvertToRadians(110.f)) * m_pTransformCom->Get_WorldMatrix();
				WorldMatrix.r[CTransform::STATE_POSITION] = m_pTransformCom->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f);
				CEffect_Manager::GetInstance()->Generate_Effect(L"Slash_0", XMMatrixIdentity(), WorldMatrix, 2.f);

				GI->Play_Sound(L"Slash_0.wav", CHANNELID::SOUND_SLASH, 0.3f, true);
			}
		}
		break;
	case 5:
		if (fProgress >= 0.2f)
		{
			if (false == m_bSlashEffect[6])
			{
				m_bSlashEffect[6] = true;
				_matrix WorldMatrix = XMMatrixRotationZ(XMConvertToRadians(20.f)) * m_pTransformCom->Get_WorldMatrix();
				WorldMatrix.r[CTransform::STATE_POSITION] = m_pTransformCom->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f);
				CEffect_Manager::GetInstance()->Generate_Effect(L"Slash_0", XMMatrixIdentity(), WorldMatrix, 2.f);

				GI->Play_Sound(L"Slash_0.wav", CHANNELID::SOUND_SLASH, 0.3f, true);
			}
		}

		if (fProgress >= 0.6f)
		{
			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);
		}
		else
		{
			m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BLOW, 0.f, 10.f, 1.f, true);
		}
		break;
	}


}

void CState_Kyojuro_Attack::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_pCharacter->SweathSword();

	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);

	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);

	for (_uint i = 0; i < 8; ++i)
	{
		m_bSlashEffect[i] = false;
	}
}


void CState_Kyojuro_Attack::Input(_float fTimeDelta)
{
	if (KEY_HOLD(KEY::LBTN))
	{
		if (m_iCurrAnimIndex <= 4)
		{
			TCHAR strSoundFileName[MAX_PATH] = L"Voice_Kyojuro_Attack_";
			lstrcatW(strSoundFileName, to_wstring(CUtils::Random_Int(0, 3)).c_str());
			lstrcatW(strSoundFileName, L".wav");
			GI->Play_Sound(strSoundFileName, CHANNELID::SOUND_VOICE_CHARACTER, 1.f);
		}

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
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
			break;
		case 3:
			Find_Near_Target();
			Trace_Near_Target();
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
			break;

		case 4:
			Find_Near_Target();
			Trace_Near_Target();
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);
			break;

		default:		
			break;
		}

		return;
	}
}

void CState_Kyojuro_Attack::Find_Near_Target()
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

void CState_Kyojuro_Attack::Trace_Near_Target()
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

CState_Kyojuro_Attack* CState_Kyojuro_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Kyojuro_Attack* pInstance =  new CState_Kyojuro_Attack(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Kyojuro_Attack");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Kyojuro_Attack::Free()
{
	__super::Free();
}

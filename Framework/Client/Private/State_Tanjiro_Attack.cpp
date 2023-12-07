#include "stdafx.h"
#include "State_Tanjiro_Attack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "Sword.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"
#include "Utils.h"

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

	for (_uint i = 0; i < 5; ++i)
		m_bSlashEffect[i] = false;

	Find_Near_Target();
	m_pCharacter->DrawSword();

	m_pSword->Set_ActiveColliders(CCollider::ATTACK, true);
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f, false);
	m_pSword->SetUp_Trail_Position(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, -1.5f, 1.f));


	for (_uint i = 0; i < 5; ++i)
	{
		m_bSlashEffect[i] = false;
	}



}

void CState_Tanjiro_Attack::Tick_State(_float fTimeDelta)
{
	if (nullptr != m_pTarget)
	{
		if (m_pTarget->Is_ReserveDead() || m_pTarget->Is_Dead())
			m_pTarget = nullptr;
	}
	

	
	Input(fTimeDelta);
	
	_float fProgress = m_pModelCom->Get_Animations()[m_AnimIndices[m_iCurrAnimIndex]]->Get_AnimationProgress();

	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[m_iCurrAnimIndex]))
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
		return;
	}
	

	switch (m_iCurrAnimIndex)
	{
	case 0:
		if (fProgress > 0.2f)
		{
			if (false == m_bSlashEffect[m_iCurrAnimIndex])
			{
				m_bSlashEffect[m_iCurrAnimIndex] = true;
				_matrix WorldMatrix = XMMatrixRotationZ(XMConvertToRadians(-120.f)) * m_pTransformCom->Get_WorldMatrix();
				WorldMatrix.r[CTransform::STATE_POSITION] = m_pTransformCom->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f);
				CEffect_Manager::GetInstance()->Generate_Effect(L"Slash_0", XMMatrixIdentity(), WorldMatrix, 2.f);

				GI->Play_Sound(L"Slash_0.wav", CHANNELID::SOUND_SLASH, 0.3f);
			}
		}
		if (fProgress > 0.5f)
		{
			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Stop_Trail();
		}
		
		m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
		m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
		break;

	case 1:
		if (fProgress > 0.2f)
		{
			if (false == m_bSlashEffect[m_iCurrAnimIndex])
			{
				m_bSlashEffect[m_iCurrAnimIndex] = true;
				_matrix WorldMatrix = XMMatrixRotationZ(XMConvertToRadians(120.f)) * m_pTransformCom->Get_WorldMatrix();
				WorldMatrix.r[CTransform::STATE_POSITION] = m_pTransformCom->Get_Position() + XMVectorSet(0.f, .5f, 0.f, 0.f);
				CEffect_Manager::GetInstance()->Generate_Effect(L"Slash_0", XMMatrixIdentity(), WorldMatrix, 2.f);

				GI->Play_Sound(L"Slash_0.wav", CHANNELID::SOUND_SLASH, 0.3f);
			}
		}

		if (fProgress > 0.5f)
		{
			m_pSword->Stop_Trail();

			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			
		}

		m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f, false);
		m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f, false);
		break;

	case 2:
		if (fProgress > 0.4f)
		{
			if (false == m_bSlashEffect[m_iCurrAnimIndex])
			{
				m_bSlashEffect[m_iCurrAnimIndex] = true;
				_matrix WorldMatrix = XMMatrixRotationZ(XMConvertToRadians(-120.f)) * m_pTransformCom->Get_WorldMatrix();
				WorldMatrix.r[CTransform::STATE_POSITION] = m_pTransformCom->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f);
				CEffect_Manager::GetInstance()->Generate_Effect(L"Slash_0", XMMatrixIdentity(), WorldMatrix, 2.f);

				GI->Play_Sound(L"Slash_0.wav", CHANNELID::SOUND_SLASH, 0.3f);
			}
		}

		if (fProgress > 0.5f)
		{
			m_pSword->Stop_Trail();

			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
		}
		m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f, false);
		m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f, false);
		break;

	case 3:
		if (fProgress >= 0.2f && fProgress <= 0.6f)
		{
			if (fProgress <= 0.4f)
			{
				if (false == m_bSlashEffect[m_iCurrAnimIndex])
				{
					m_bSlashEffect[m_iCurrAnimIndex] = true;
					_matrix WorldMatrix = XMMatrixRotationZ(XMConvertToRadians(120.f)) * m_pTransformCom->Get_WorldMatrix();
					WorldMatrix.r[CTransform::STATE_POSITION] = m_pTransformCom->Get_Position() + XMVectorSet(0.f, .5f, 0.f, 0.f);
					CEffect_Manager::GetInstance()->Generate_Effect(L"Slash_0", XMMatrixIdentity(), WorldMatrix, 2.f);
					GI->Play_Sound(L"Slash_0.wav", CHANNELID::SOUND_SLASH, 0.3f);

				}
				m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 12.f, 0.1f, 1.f);
				m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 12.f, 0.1f, 1.f);
			}
			else
			{
				m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BOUND, -5.f, 1.5f, 1.f, false);
				m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BOUND, -5.f, 1.5f, 1.f, false);
			}
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
		}
		else
		{
			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
		}
		break;

	case 4:
		if (fProgress >= 0.2f && fProgress <= 0.55f)
		{
			if (false == m_bSlashEffect[m_iCurrAnimIndex])
			{
				m_bSlashEffect[m_iCurrAnimIndex] = true;
				_matrix WorldMatrix = XMMatrixRotationZ(XMConvertToRadians(20.f)) * m_pTransformCom->Get_WorldMatrix();
				WorldMatrix.r[CTransform::STATE_POSITION] = m_pTransformCom->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f);
				CEffect_Manager::GetInstance()->Generate_Effect(L"Slash_0", XMMatrixIdentity(), WorldMatrix, 2.f);

				GI->Play_Sound(L"Slash_0.wav", CHANNELID::SOUND_SLASH, 0.3f);
			}

			m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BLOW, 0.f, 10.f, 1.f);
			m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BLOW, 0.f, 10.f, 1.f);
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
		}
		else
		{
			if (fProgress > 0.55f)
			{
				m_pSword->Stop_Trail();
			}
			m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
			m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
		}
			
		break;
	}

}

void CState_Tanjiro_Attack::Exit_State()
{
	for (_uint i = 0; i < 5; ++i)	
		m_bSlashEffect[i] = false;

	m_iCurrAnimIndex = 0;
	
	Find_Near_Target();

	m_pSword->Stop_Trail();

	m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, false);
	m_pSword->Set_ActiveColliders(CCollider::ATTACK, false);

	m_pCharacter->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f, false);
	m_pSword->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f, false);

	for (_uint i = 0; i < 5; ++i)
	{
		m_bSlashEffect[i] = false;
	}


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

				
			TCHAR strSoundFileName[MAX_PATH] = L"Voice_Tanjiro_Attack_";
			lstrcatW(strSoundFileName, to_wstring(CUtils::Random_Int(0, 15)).c_str());
			lstrcatW(strSoundFileName, L".wav");
			GI->Play_Sound(strSoundFileName, CHANNELID::SOUND_VOICE_CHARACTER, 1.f);

			if (m_iCurrAnimIndex != m_AnimIndices.size())
				m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);

			
			switch (m_iCurrAnimIndex)
			{
			case 1:
				Find_Near_Target();
				Trace_Near_Target();
				m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);

				
				break;

			case 2:
				Find_Near_Target();
				Trace_Near_Target();
				m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);

				
				break;

			case 3:
				Find_Near_Target();
				Trace_Near_Target();
				m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
				break;

			case 4:
				Find_Near_Target();
				Trace_Near_Target();
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

void CState_Tanjiro_Attack::Trace_Near_Target()
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

#include "stdafx.h"
#include "State_Character_Battle_Jump.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Sword.h"
#include "Animation.h"
#include "RigidBody.h"
#include "StateMachine.h"



CState_Character_Battle_Jump::CState_Character_Battle_Jump(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Character_Battle_Jump::Initialize(const list<wstring>& AnimationList)
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

void CState_Character_Battle_Jump::Enter_State(void* pArg)
{

	m_iCurrAnimIndex = 0;
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);

	if (CSword::ZENITSU != m_pSword->Get_SwordType())
		m_pCharacter->DrawSword();
	else
		m_pCharacter->SweathSword();

	m_pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

	m_iCurrAnimIndex = 0;
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	
	float fDirX = XMVectorGetX(vLook);
	fDirX = max(fDirX, -1.f);
	fDirX = min(fDirX, 1.f);

	float fDirZ = XMVectorGetZ(vLook);
	fDirZ = max(fDirZ, -1.f);
	fDirZ = min(fDirZ, 1.f);


	_vector vJumpDir = XMVectorSet(fDirX, 0.8f, fDirZ, 0.f);

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition = XMVectorSetY(vPosition, XMVectorGetY(vPosition) + 0.1f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(vJumpDir), 12.f);

	switch (m_pCharacter->Get_CharacterType())
	{
	case CCharacter::CHARACTER_TYPE::TANJIRO:
		GI->Play_Sound(L"Voice_Tanjiro_Jump.wav", CHANNELID::SOUND_VOICE_CHARACTER, 1.f);
		break;

	case CCharacter::CHARACTER_TYPE::ZENITSU:
		GI->Play_Sound(L"Voice_Zenitsu_Jump.wav", CHANNELID::SOUND_VOICE_CHARACTER, 1.f);
		break;

	case CCharacter::CHARACTER_TYPE::KYOJURO:
		GI->Play_Sound(L"Voice_Kyojuro_Jump.wav", CHANNELID::SOUND_VOICE_CHARACTER, 1.f);
		break;
	}
}

void CState_Character_Battle_Jump::Tick_State(_float fTimeDelta)
{
	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[0]) || m_pModelCom->Is_Animation_Finished(m_AnimIndices[1]))
	{
		m_iCurrAnimIndex++;
		m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	}


	// if(m_pRigidBody->)
	if (m_pRigidBodyCom->Is_Ground())
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
		if (LEVELID::LEVEL_TRAIN_STATION == GI->Get_CurrentLevel() || LEVELID::LEVEL_FINAL_BOSS == GI->Get_CurrentLevel())
		{
			GI->Play_Sound(L"Foot_Dirt_0.wav", CHANNELID::SOUND_FOOT_CHARACTER, 0.3f);
		}
		else if (LEVELID::LEVEL_TRAIN == GI->Get_CurrentLevel())
		{
			GI->Play_Sound(L"Foot_Floor_0.wav", CHANNELID::SOUND_FOOT_CHARACTER, 0.3f);
		}
		else if(LEVELID::LEVEL_TRAIN_BOSS == GI->Get_CurrentLevel())
		{
			GI->Play_Sound(L"Foot_Train_0.wav", CHANNELID::SOUND_FOOT_CHARACTER, 0.3f);
		}

		return;
	}
		

	Input();
}

void CState_Character_Battle_Jump::Exit_State()
{
	m_iCurrAnimIndex = 0;
}

void CState_Character_Battle_Jump::Input()
{
	if (KEY_TAP(KEY::RBTN) && KEY_HOLD(KEY::A))
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_AIRDASH);
		return;
	}

	if (KEY_TAP(KEY::RBTN) && KEY_HOLD(KEY::D))
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_AIRDASH);
		return;
	}

	if (KEY_HOLD(KEY::LBTN))
	{
		m_pStateMachineCom->Change_State(CCharacter::AIR_ATTACK);
		return;
	}
}

CState_Character_Battle_Jump* CState_Character_Battle_Jump::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Character_Battle_Jump* pInstance =  new CState_Character_Battle_Jump(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Character_Battle_Jump");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Character_Battle_Jump::Free()
{
	__super::Free();
}

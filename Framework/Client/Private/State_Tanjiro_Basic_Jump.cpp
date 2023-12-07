#include "stdafx.h"
#include "State_Tanjiro_Basic_Jump.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "RigidBody.h"
#include "StateMachine.h"
#include "Sword.h"

USING(Client)
CState_Tanjiro_Basic_Jump::CState_Tanjiro_Basic_Jump(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Tanjiro_Basic_Jump::Initialize(const list<wstring>& AnimationList)
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

void CState_Tanjiro_Basic_Jump::Enter_State(void* pArg)
{

	m_pCharacter->SweathSword();
	m_pSword->Stop_Trail();
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

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

	m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(vJumpDir), 15.f);



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

void CState_Tanjiro_Basic_Jump::Tick_State(_float fTimeDelta)
{
	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[0]))
	{
		m_iCurrAnimIndex = 1;
		m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
	}

	// if(m_pRigidBody->)
	if (m_pRigidBodyCom->Is_Ground())
	{
		m_pStateMachineCom->Change_State(CCharacter::BASIC_IDLE);
	}

		
}

void CState_Tanjiro_Basic_Jump::Exit_State()
{

}

CState_Tanjiro_Basic_Jump* CState_Tanjiro_Basic_Jump::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Tanjiro_Basic_Jump* pInstance =  new CState_Tanjiro_Basic_Jump(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Tanjiro_Basic_Jump");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Tanjiro_Basic_Jump::Free()
{
	__super::Free();
}

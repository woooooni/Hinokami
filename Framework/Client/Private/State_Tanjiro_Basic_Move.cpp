#include "stdafx.h"
#include "State_Tanjiro_Basic_Move.h"
#include "GameInstance.h"
#include "Tanjiro.h"
#include "PipeLine.h"
#include "StateMachine.h"
#include "Navigation.h"
#include "Sword.h"
#include "Animation.h"

USING(Client)
CState_Tanjiro_Basic_Move::CState_Tanjiro_Basic_Move(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Tanjiro_Basic_Move::Initialize(const list<wstring>& AnimationList)
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

void CState_Tanjiro_Basic_Move::Enter_State(void* pArg)
{

	m_pCharacter->SweathSword();
	m_pSword->Stop_Trail();

	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

	m_iCurrAnimIndex = 0;
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);

	m_fMoveSpeed = m_pTransformCom->Get_TickPerSecond();
}

void CState_Tanjiro_Basic_Move::Tick_State(_float fTimeDelta)
{
	_bool bKeyHolding = false;

	if (KEY_TAP(KEY::SHIFT))
	{
		m_iCurrAnimIndex = m_AnimIndices[1];
		m_pModelCom->Set_AnimIndex(m_AnimIndices[1]);
		m_pTransformCom->Set_TickPerSecond(m_fMoveSpeed + 5.f);
	}

	if (KEY_AWAY(KEY::SHIFT))
	{
		m_iCurrAnimIndex = m_AnimIndices[0];
		m_pModelCom->Set_AnimIndex(m_iCurrAnimIndex);
		m_pTransformCom->Set_TickPerSecond(m_pTransformCom->Get_TickPerSecond() - 5.f);
	}
	
	if (KEY_HOLD(KEY::W))
	{
		bKeyHolding = true;
		

		_matrix vCamWolrd = GI->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);

		_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_vector vCamLook = vCamWolrd.r[CTransform::STATE_LOOK];

		vRight = XMVector3Normalize(vRight);
		vCamLook = XMVector3Normalize(vCamLook);

		_float fRadian = XMVectorGetX(XMVector3Dot(vRight, vCamLook)) * 10.f * fTimeDelta;


		m_pTransformCom->Rotation_Acc(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	}
		

	if (KEY_HOLD(KEY::S))
	{
		bKeyHolding = true;


		_matrix vCamWolrd = GI->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);

		_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_vector vCamLook = vCamWolrd.r[CTransform::STATE_LOOK];

		vRight = XMVector3Normalize(vRight);
		vCamLook = -1.f * XMVector3Normalize(vCamLook);

		_float fRadian = XMVectorGetX(XMVector3Dot(vRight, vCamLook)) * 10.f * fTimeDelta;


		m_pTransformCom->Rotation_Acc(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	}
		

	if (KEY_HOLD(KEY::A))
	{
		_matrix vCamWolrd = GI->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);

		_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_vector vCamRight = vCamWolrd.r[CTransform::STATE_RIGHT];

		vRight = XMVector3Normalize(vRight);
		vCamRight = -1.f * XMVector3Normalize(vCamRight);

		_float fRadian = XMVectorGetX(XMVector3Dot(vRight, vCamRight)) * 10.f * fTimeDelta;

		m_pTransformCom->Rotation_Acc(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);

		if (!bKeyHolding)
		{
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		}
		
		bKeyHolding = true;
		
	}
		

	if (KEY_HOLD(KEY::D))
	{
		// m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 10.f, -1.f * fTimeDelta);

		_matrix vCamWolrd = GI->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);

		_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_vector vCamRight = vCamWolrd.r[CTransform::STATE_RIGHT];

		vRight = XMVector3Normalize(vRight);
		vCamRight = XMVector3Normalize(vCamRight);

		_float fRadian = XMVectorGetX(XMVector3Dot(vRight, vCamRight)) * 10.f * fTimeDelta;


		m_pTransformCom->Rotation_Acc(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);

		if(!bKeyHolding)
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

		bKeyHolding = true;
	}

	if (KEY_TAP(KEY::LBTN))
	{
		bKeyHolding = true;
		m_pStateMachineCom->Change_State(CCharacter::ATTACK);
	}


	if (KEY_TAP(KEY::SPACE))
	{
		bKeyHolding = true;
		m_pStateMachineCom->Change_State(CCharacter::BASIC_JUMP);
	}
		
		
		
	if (!bKeyHolding)
	{
		if (KEY_NONE(KEY::W) && KEY_NONE(KEY::A) && KEY_NONE(KEY::S) && KEY_NONE(KEY::D))		
			m_pStateMachineCom->Change_State(CTanjiro::BASIC_IDLE);
	}

	_float fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();
	fProgress *= 100.f;
	fProgress = round(fProgress) / 100.f;

	switch (GI->Get_CurrentLevel())
	{
	case LEVELID::LEVEL_TRAIN_STATION:
	case LEVELID::LEVEL_FINAL_BOSS:
		if (fProgress >= 0.6f && fProgress <= 0.61f)
		{
			GI->Play_Sound(L"Foot_Dirt_1.wav", CHANNELID::SOUND_FOOT_CHARACTER, 0.3f, false);
			break;
		}
		else if (fProgress >= 0.2f && fProgress <= 0.21f)
		{
			GI->Play_Sound(L"Foot_Dirt_0.wav", CHANNELID::SOUND_FOOT_CHARACTER, 0.3f, false);
			break;
		}
		break;

	case LEVELID::LEVEL_TRAIN_BOSS:
		if (fProgress >= 0.6f && fProgress <= 0.61f)
		{
			GI->Play_Sound(L"Foot_Train_1.wav", CHANNELID::SOUND_FOOT_CHARACTER, 0.3f, false);
			break;
		}
		else if (fProgress >= 0.2f && fProgress <= 0.21f)
		{
			GI->Play_Sound(L"Foot_Train_0.wav", CHANNELID::SOUND_FOOT_CHARACTER, 0.3f, false);
			break;
		}
		break;

	case LEVELID::LEVEL_TRAIN:
		if (fProgress >= 0.6f && fProgress <= 0.61f)
		{
			GI->Play_Sound(L"Foot_Floor_1.wav", CHANNELID::SOUND_FOOT_CHARACTER, 0.3f, false);
			break;
		}
		else if (fProgress >= 0.2f && fProgress <= 0.21f)
		{
			GI->Play_Sound(L"Foot_Floor_0.wav", CHANNELID::SOUND_FOOT_CHARACTER, 0.3f, false);
			break;
		}
		break;
	}
	
}

void CState_Tanjiro_Basic_Move::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_pTransformCom->Set_TickPerSecond(m_fMoveSpeed);
}

CState_Tanjiro_Basic_Move* CState_Tanjiro_Basic_Move::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Tanjiro_Basic_Move* pInstance =  new CState_Tanjiro_Basic_Move(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Tanjiro_Basic_Move");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Tanjiro_Basic_Move::Free()
{
	__super::Free();
}

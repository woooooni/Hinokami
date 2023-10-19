#include "stdafx.h"
#include "State_Tanjiro_Run.h"
#include "GameInstance.h"
#include "Model.h"
#include "Tanjiro.h"


CState_Tanjiro_Run::CState_Tanjiro_Run(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTransform* pTransform, CStateMachine* pStateMachine, CModel* pModel)
	: CState(pStateMachine, pModel, pTransform)
{

}

HRESULT CState_Tanjiro_Run::Initialize(const list<wstring>& AnimationList)
{
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

void CState_Tanjiro_Run::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = m_AnimationIndices[0];
	m_pModelCom->Set_AnimIndex(m_AnimationIndices[0]);
}

void CState_Tanjiro_Run::Tick_State(_float fTimeDelta)
{
	_bool bKeyHolding = false;

	if (KEY_TAP(KEY::SHIFT))
	{
		m_iCurrAnimIndex = m_AnimationIndices[1];
		m_pModelCom->Set_AnimIndex(m_AnimationIndices[1]);
		m_pTransformCom->Set_TickPerSecond(m_pTransformCom->Get_TickPerSecond() * 1.5f);
	}

	if (KEY_AWAY(KEY::SHIFT))
	{
		m_iCurrAnimIndex = m_AnimationIndices[1];
		m_pModelCom->Set_AnimIndex(m_AnimationIndices[1]);
		m_pTransformCom->Set_TickPerSecond(m_pTransformCom->Get_TickPerSecond() * 0.5f);
	}
	
	if (KEY_HOLD(KEY::W))
	{
		bKeyHolding = true;
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
		

	if (KEY_HOLD(KEY::S))
	{
		bKeyHolding = true;
		m_pTransformCom->Go_Backward(fTimeDelta);
	}
		

	if (KEY_HOLD(KEY::A))
	{
		bKeyHolding = true;
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -1.f * fTimeDelta);
	}
		

	if (KEY_HOLD(KEY::D))
	{
		bKeyHolding = true;
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	}

	if (KEY_TAP(KEY::LBTN))
	{
		bKeyHolding = true;
		m_pStateMachineCom->Change_State(CCharacter::STATE_ATTACK);
	}
		
		
	if (!bKeyHolding)
	{
		if (KEY_NONE(KEY::W) && KEY_NONE(KEY::A) && KEY_NONE(KEY::S) && KEY_NONE(KEY::D))		
			m_pStateMachineCom->Change_State(CTanjiro::STATE_IDLE);
	}
	
}

void CState_Tanjiro_Run::Exit_State()
{
	m_iCurrAnimIndex = 0;
}

CState_Tanjiro_Run* CState_Tanjiro_Run::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTransform* pTransform, CStateMachine* pStateMachine,
	CModel* pModel,const list<wstring>& AnimationList)
{
	CState_Tanjiro_Run* pInstance =  new CState_Tanjiro_Run(pDevice, pContext, pTransform, pStateMachine, pModel);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Tanjiro_Run");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Tanjiro_Run::Free()
{
}

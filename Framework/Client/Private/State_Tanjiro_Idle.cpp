#include "stdafx.h"
#include "State_Tanjiro_Idle.h"
#include "GameInstance.h"
#include "Model.h"


CState_Tanjiro_Idle::CState_Tanjiro_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTransform* pTransform, CStateMachine* pStateMachine, CModel* pModel)
	: CState(pStateMachine, pModel, pTransform)
{

}

HRESULT CState_Tanjiro_Idle::Initialize(const list<wstring>& AnimationList)
{
	for (auto strAnimName : AnimationList)
	{
		_int iAnimIndex = m_pModelCom->Find_AnimationIndex(strAnimName);
		if (-1 != iAnimIndex)
		{
			m_AnimationIndices.push_back(iAnimIndex);
		}
		else
		{
			return E_FAIL;
		}
	}
	
	return S_OK;
}

void CState_Tanjiro_Idle::Enter_State()
{
	m_pModelCom->Set_AnimIndex(m_iCurrAnimIndex);
}

void CState_Tanjiro_Idle::Tick_State(_float fTimeDelta)
{

}

void CState_Tanjiro_Idle::Exit_State()
{
}

CState_Tanjiro_Idle* CState_Tanjiro_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTransform* pTransform, CStateMachine* pStateMachine,
	CModel* pModel,const list<wstring>& AnimationList)
{
	CState_Tanjiro_Idle* pInstance =  new CState_Tanjiro_Idle(pDevice, pContext, pTransform, pStateMachine, pModel);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Tanjiro_Idle");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Tanjiro_Idle::Free()
{
}

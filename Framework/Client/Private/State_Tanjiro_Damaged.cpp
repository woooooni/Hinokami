#include "stdafx.h"
#include "State_Tanjiro_Damaged.h"
#include "GameInstance.h"
#include "Model.h"


CState_Tanjiro_Damaged::CState_Tanjiro_Damaged(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTransform* pTransform, CStateMachine* pStateMachine, CModel* pModel)
	: CState(pStateMachine, pModel, pTransform)
{

}

HRESULT CState_Tanjiro_Damaged::Initialize(const list<wstring>& AnimationList)
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

void CState_Tanjiro_Damaged::Enter_State(void* pArg)
{
	m_pModelCom->Set_AnimIndex(m_AnimationIndices[0]);
}

void CState_Tanjiro_Damaged::Tick_State(_float fTimeDelta)
{
	
}

void CState_Tanjiro_Damaged::Exit_State()
{
}

CState_Tanjiro_Damaged* CState_Tanjiro_Damaged::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTransform* pTransform, CStateMachine* pStateMachine,
	CModel* pModel,const list<wstring>& AnimationList)
{
	CState_Tanjiro_Damaged* pInstance =  new CState_Tanjiro_Damaged(pDevice, pContext, pTransform, pStateMachine, pModel);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Tanjiro_Damaged");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Tanjiro_Damaged::Free()
{
}

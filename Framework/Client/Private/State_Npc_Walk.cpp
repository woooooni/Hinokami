#include "stdafx.h"
#include "State_Npc_Walk.h"
#include "GameInstance.h"
#include "Model.h"
#include "Npc.h"

CState_Npc_Walk::CState_Npc_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Npc_Walk::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;
	
	return S_OK;
}

void CState_Npc_Walk::Enter_State(void* pArg)
{
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pModelCom->Set_AnimIndex(m_AnimIndices[0]);
}

void CState_Npc_Walk::Tick_State(_float fTimeDelta)
{
	
}

void CState_Npc_Walk::Exit_State()
{
	
}

CState_Npc_Walk* CState_Npc_Walk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Npc_Walk* pInstance =  new CState_Npc_Walk(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Npc_Walk");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Npc_Walk::Free()
{
	__super::Free();
}

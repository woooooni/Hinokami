#include "stdafx.h"
#include "State_Npc_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Npc.h"

CState_Npc_Idle::CState_Npc_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Npc_Idle::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;
	
	return S_OK;
}

void CState_Npc_Idle::Enter_State(void* pArg)
{
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pModelCom->Set_AnimIndex(m_AnimIndices[0]);

}

void CState_Npc_Idle::Tick_State(_float fTimeDelta)
{
	
}

void CState_Npc_Idle::Exit_State()
{
	
}

CState_Npc_Idle* CState_Npc_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Npc_Idle* pInstance =  new CState_Npc_Idle(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Npc_Idle");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Npc_Idle::Free()
{
	__super::Free();
}

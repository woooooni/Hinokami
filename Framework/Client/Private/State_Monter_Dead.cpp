#include "stdafx.h"
#include "State_Monter_Dead.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"


CState_Monster_Dead::CState_Monster_Dead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Monster_Dead::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;
	
	return S_OK;
}

void CState_Monster_Dead::Enter_State(void* pArg)
{
	m_pModelCom->Set_AnimIndex(m_AnimIndices[0]);
}

void CState_Monster_Dead::Tick_State(_float fTimeDelta)
{
	
}

void CState_Monster_Dead::Exit_State()
{
	
}

CState_Monster_Dead* CState_Monster_Dead::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Monster_Dead* pInstance =  new CState_Monster_Dead(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Monster_Dead");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Monster_Dead::Free()
{
	__super::Free();
}

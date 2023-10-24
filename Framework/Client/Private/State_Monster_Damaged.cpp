#include "stdafx.h"
#include "State_Monster_Damaged.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"


CState_Monster_Damaged::CState_Monster_Damaged(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Monster_Damaged::Initialize(const list<wstring>& AnimationList)
{
	m_pModelCom = m_pStateMachineCom->Get_Owner()->Get_Component<CModel>(L"Com_Model");
	if (nullptr == m_pModelCom)
		return E_FAIL;


	m_pTransformCom = m_pStateMachineCom->Get_Owner()->Get_Component<CTransform>(L"Com_Transform");
	if (nullptr == m_pTransformCom)
		return E_FAIL;


	Safe_AddRef(m_pModelCom);
	Safe_AddRef(m_pTransformCom);

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

void CState_Monster_Damaged::Enter_State(void* pArg)
{
	m_pModelCom->Set_AnimIndex(m_AnimationIndices[0]);
}

void CState_Monster_Damaged::Tick_State(_float fTimeDelta)
{
	
}

void CState_Monster_Damaged::Exit_State()
{
}

CState_Monster_Damaged* CState_Monster_Damaged::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Monster_Damaged* pInstance =  new CState_Monster_Damaged(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Monster_Damaged");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Monster_Damaged::Free()
{
	__super::Free();
}

#include "State.h"
#include "Model.h"
#include "GameObject.h"
#include "StateMachine.h"
#include "Animation.h"



CState::CState(CStateMachine* pStateMachine)
	: m_pStateMachineCom(pStateMachine)
{	
	Safe_AddRef(m_pStateMachineCom);
}

HRESULT CState::Initialize(const list<wstring>& AnimationList)
{
	m_pOwner = m_pStateMachineCom->Get_Owner();
	if (nullptr == m_pOwner)
		return E_FAIL;

	m_pModelCom = m_pStateMachineCom->Get_Owner()->Get_Component<CModel>(L"Com_Model");
	if (nullptr == m_pModelCom)
		return E_FAIL;

	m_pRigidBodyCom = m_pStateMachineCom->Get_Owner()->Get_Component<CRigidBody>(L"Com_RigidBody");
	if (nullptr == m_pRigidBodyCom)
		return E_FAIL;

	m_pTransformCom = m_pStateMachineCom->Get_Owner()->Get_Component<CTransform>(L"Com_Transform");
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_pNavigationCom = m_pStateMachineCom->Get_Owner()->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr == m_pNavigationCom)
		return E_FAIL;


	m_AnimIndices.reserve(AnimationList.size());
	for (auto strAnimName : AnimationList)
	{
		_int iAnimIndex = m_pModelCom->Find_AnimationIndex(strAnimName);
		if (-1 != iAnimIndex)
			m_AnimIndices.push_back(iAnimIndex);
		else
			return E_FAIL;
	}


	return S_OK;
}


void CState::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pStateMachineCom);
}

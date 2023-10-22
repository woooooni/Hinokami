#include "StateMachine.h"
#include "State.h"
CStateMachine::CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
}

CStateMachine::CStateMachine(const CStateMachine& rhs)
    : CComponent(rhs)
{

}

HRESULT CStateMachine::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CStateMachine::Initialize(void* pArg)
{
    return S_OK;
}

void CStateMachine::Tick_State(_float fTimeDelta)
{
	if (nullptr == m_pCurrState)
		return;

	m_pCurrState->Tick_State(fTimeDelta);
}

HRESULT CStateMachine::Add_State(_uint eStateTag, CState* pState)
{
	if (nullptr != Find_State(eStateTag))
		return E_FAIL;


	m_States.emplace(eStateTag, pState);

	return S_OK;
}

HRESULT CStateMachine::Change_State(_uint eStateTag, void* pArg)
{
	CState* pState = Find_State(eStateTag);
	if (nullptr == pState)
		return E_FAIL;

	if (nullptr != m_pCurrState)
		m_pCurrState->Exit_State();

	m_iCurrState = eStateTag;
	m_pCurrState = pState;
	m_pCurrState->Enter_State(pArg);

	return S_OK;
}




CState* CStateMachine::Find_State(const _uint eState)
{
	auto iter = m_States.find(eState);

	if (iter == m_States.end())
		return nullptr;

	return iter->second;
}

CStateMachine* CStateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStateMachine* pInstance = new CStateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CStateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CStateMachine::Clone(void* pArg)
{
	CStateMachine* pInstance = new CStateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CStateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CStateMachine::Free()
{
	for (auto& State : m_States)
		Safe_Release(State.second);
	m_States.clear();

	__super::Free();
}

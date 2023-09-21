#include "State.h"
#include "StateMachine.h"

CState::CState(const wstring& strStateName, const wstring& strAnimationKey)
	: m_strStateName(strStateName)
	, m_strAnimationKey(strAnimationKey)
{

}

CState::CState(const CState& rhs, CStateMachine* pMachine)
	: m_strStateName(rhs.m_strStateName)
	, m_strAnimationKey(rhs.m_strAnimationKey)
	, m_pStateMachine(pMachine)

{

}	


void CState::Free()
{
	__super::Free();
}

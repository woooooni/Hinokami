#include "State.h"
#include "StateMachine.h"
#include "Animation.h"


CState::CState(CStateMachine* pStateMachine)
	: m_pStateMachineCom(pStateMachine)
{	
	Safe_AddRef(m_pStateMachineCom);
}


void CState::Free()
{
	__super::Free();
	Safe_Release(m_pStateMachineCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
}

#include "State.h"
#include "StateMachine.h"
#include "Animation.h"


CState::CState(CStateMachine* pStateMachine, CModel* pModel, CTransform* pTransform)
	: m_pStateMachineCom(pStateMachine)
	, m_pModelCom(pModel)
	, m_pTransformCom(pTransform)
{

	Safe_AddRef(m_pModelCom);
	Safe_AddRef(m_pStateMachineCom);
	Safe_AddRef(m_pTransformCom);
}


void CState::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pStateMachineCom);
}

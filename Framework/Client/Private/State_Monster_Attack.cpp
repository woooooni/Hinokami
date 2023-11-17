#include "stdafx.h"
#include "State_Monster_Attack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Navigation.h"
#include "Animation.h"

CState_Monster_Attack::CState_Monster_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Monster_Attack::Initialize(const list<wstring>& AnimationList)
{
	m_pModelCom = m_pStateMachineCom->Get_Owner()->Get_Component<CModel>(L"Com_Model");
	if (nullptr == m_pModelCom)
		return E_FAIL;


	m_pTransformCom = m_pStateMachineCom->Get_Owner()->Get_Component<CTransform>(L"Com_Transform");
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_pNavigationCom = m_pStateMachineCom->Get_Owner()->Get_Component<CNavigation>(L"Com_Navigation");


	Safe_AddRef(m_pModelCom);
	Safe_AddRef(m_pTransformCom);
	Safe_AddRef(m_pNavigationCom);

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

void CState_Monster_Attack::Enter_State(void* pArg)
{
	m_iRandomIndex = rand() % m_AnimationIndices.size();
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
	m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iRandomIndex]);
}

void CState_Monster_Attack::Tick_State(_float fTimeDelta)
{
	_vector vLook = m_pTransformCom->Get_WorldMatrix().r[CTransform::STATE_LOOK];
	_vector vPosition = m_pTransformCom->Get_WorldMatrix().r[CTransform::STATE_POSITION];

	vLook = XMVector3Normalize(vLook);
	vPosition += vLook * 1.f;

	_bool bMovable = false;
	m_pTransformCom->Set_Position(vLook, m_pNavigationCom, &bMovable);

	if (m_pModelCom->Is_Animation_Finished(m_AnimationIndices[m_iRandomIndex]))
	{
		m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
	}
}

void CState_Monster_Attack::Exit_State()
{
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_iCurrAnimIndex = 0;
	m_iRandomIndex = 0;
}


CState_Monster_Attack* CState_Monster_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Monster_Attack* pInstance =  new CState_Monster_Attack(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Monster_Attack");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Monster_Attack::Free()
{
	__super::Free();
	Safe_Release(m_pNavigationCom);
}

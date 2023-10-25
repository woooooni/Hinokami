#include "stdafx.h"
#include "State_Monster_Attack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
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

void CState_Monster_Attack::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = 0;
	m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iCurrAnimIndex]);
}

void CState_Monster_Attack::Tick_State(_float fTimeDelta)
{
	if (m_pModelCom->Is_Animation_Finished(m_AnimationIndices[m_iCurrAnimIndex]))
	{
		m_iCurrAnimIndex = min(m_iCurrAnimIndex + 1, m_AnimationIndices.size() - 1);
		if (m_iCurrAnimIndex == m_AnimationIndices.size() - 1)
		{
			m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
			return;
		}
		m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iCurrAnimIndex]);
	}
		
}

void CState_Monster_Attack::Exit_State()
{
	m_iCurrAnimIndex = 0;
}

void CState_Monster_Attack::Add_Force(_float fForce)
{
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vLook = XMVectorSetY(vLook, 0.f);

	CRigidBody* pRigid = m_pStateMachineCom->Get_Owner()->Get_Component<CRigidBody>(L"Com_RigidBody");
	if (nullptr != pRigid)
		pRigid->Add_Velocity_Acc(vLook, fForce);
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
}

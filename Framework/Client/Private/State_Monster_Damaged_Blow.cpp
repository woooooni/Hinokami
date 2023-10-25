#include "stdafx.h"
#include "State_Monster_Damaged_Blow.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"


CState_Monster_Damaged_Blow::CState_Monster_Damaged_Blow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Monster_Damaged_Blow::Initialize(const list<wstring>& AnimationList)
{
	m_pModelCom = m_pStateMachineCom->Get_Owner()->Get_Component<CModel>(L"Com_Model");
	if (nullptr == m_pModelCom)
		return E_FAIL;


	m_pTransformCom = m_pStateMachineCom->Get_Owner()->Get_Component<CTransform>(L"Com_Transform");
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_pRigidBodyCom = m_pStateMachineCom->Get_Owner()->Get_Component<CRigidBody>(L"Com_RigidBody");
	if (nullptr == m_pRigidBodyCom)
		return E_FAIL;

	Safe_AddRef(m_pRigidBodyCom);
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

void CState_Monster_Damaged_Blow::Enter_State(void* pArg)
{
	_vector vBlowDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -1);
	XMVectorSetY(vBlowDir, 0.f);
	m_pRigidBodyCom->Add_Velocity(vBlowDir, 10.f);
	m_fAccRecovery = 0.f;
	m_pModelCom->Set_AnimIndex(m_AnimationIndices[0]);
}

void CState_Monster_Damaged_Blow::Tick_State(_float fTimeDelta)
{
	if (m_pModelCom->Is_Animation_Finished(m_AnimationIndices[0]))
	{
		m_pModelCom->Set_AnimIndex(m_AnimationIndices[1]);
	}

	if (0.5f > XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pRigidBodyCom->Get_Velocity()))))
	{
		if (!m_bFirstGround)
		{
			m_bFirstGround = true;
			m_pModelCom->Set_AnimIndex(m_AnimationIndices[2]);
		}

		m_fAccRecovery += fTimeDelta;
		if (m_fAccRecovery >= m_fRecoveryTime)
		{
			m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
		}
	}
	
}

void CState_Monster_Damaged_Blow::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_bFirstGround = false;
	m_fAccRecovery = 0.f;
}

CState_Monster_Damaged_Blow* CState_Monster_Damaged_Blow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Monster_Damaged_Blow* pInstance =  new CState_Monster_Damaged_Blow(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Monster_Damaged_Blow");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Monster_Damaged_Blow::Free()
{
	__super::Free();
	Safe_Release(m_pRigidBodyCom);
}

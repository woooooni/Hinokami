#include "stdafx.h"
#include "State_Monster_Damaged_AirStay.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "RigidBody.h"
#include "Animation.h"

CState_Monster_Damaged_AirStay::CState_Monster_Damaged_AirStay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Monster_Damaged_AirStay::Initialize(const list<wstring>& AnimationList)
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

	m_pOwnerMonster = dynamic_cast<CMonster*>(m_pStateMachineCom->Get_Owner());
	if (nullptr == m_pOwnerMonster)
		return E_FAIL;

	Safe_AddRef(m_pOwnerMonster);
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

void CState_Monster_Damaged_AirStay::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = 0;
	m_fAccRecovery = 0.f;
	m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iCurrAnimIndex]);
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	
	m_pRigidBodyCom->Set_Gravity(false);
}

void CState_Monster_Damaged_AirStay::Tick_State(_float fTimeDelta)
{
	_float fProgress = m_pModelCom->Get_Animations()[m_AnimationIndices[m_iCurrAnimIndex]]->Get_AnimationProgress();
	if (fProgress >= 0.8f && m_iCurrAnimIndex == 1)
	{
		m_pRigidBodyCom->Set_Gravity(true);
	}


	if (m_pModelCom->Is_Animation_Finished(m_AnimationIndices[0]))
	{
		m_iCurrAnimIndex = 1;
		m_pModelCom->Set_AnimIndex(m_AnimationIndices[1]);
	}

	if (m_pRigidBodyCom->Is_Ground())
	{
		if (!m_bFirstGround)
		{
			m_bFirstGround = true;
			m_iCurrAnimIndex = 2;
			m_pModelCom->Set_AnimIndex(m_AnimationIndices[2]);
		}
			
		m_fAccRecovery += fTimeDelta;
		if(m_fAccRecovery >= m_fRecoveryTime)
		{
			m_pStateMachineCom->Change_State(CMonster::IDLE);
		}
	}
}

void CState_Monster_Damaged_AirStay::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_fAccRecovery = 0.f;
	m_bFirstGround = false;

	m_pRigidBodyCom->Set_Gravity(true);
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pOwnerMonster->Set_Infinite(0.5f, false);
}

CState_Monster_Damaged_AirStay* CState_Monster_Damaged_AirStay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Monster_Damaged_AirStay* pInstance =  new CState_Monster_Damaged_AirStay(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Monster_Damaged_AirStay");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Monster_Damaged_AirStay::Free()
{
	__super::Free();
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pOwnerMonster);
}

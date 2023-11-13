#include "stdafx.h"
#include "State_Monster_Damaged_Bound.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "RigidBody.h"


CState_Monster_Damaged_Bound::CState_Monster_Damaged_Bound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Monster_Damaged_Bound::Initialize(const list<wstring>& AnimationList)
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

void CState_Monster_Damaged_Bound::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = 0;
	m_fAccRecovery = 0.f;
	m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iCurrAnimIndex]);
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	
	m_pRigidBodyCom->Set_Ground(false);
	m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(XMVectorSet(0.f, -1.f, 0.f, 0.f)), 1.5f);
}

void CState_Monster_Damaged_Bound::Tick_State(_float fTimeDelta)
{
	if (m_pModelCom->Is_Animation_Finished(m_AnimationIndices[0]))
	{
		m_pModelCom->Set_AnimIndex(m_AnimationIndices[1]);
	}

	if (m_pRigidBodyCom->Is_Ground())
	{
		if (!m_bFirstGround)
		{
			m_bFirstGround = true;

			_vector vPosition = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) + .5f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

			m_pModelCom->Set_AnimIndex(m_AnimationIndices[0]);

			m_pRigidBodyCom->Set_Velocity({ 0.f, 0.f, 0.f });
			m_pRigidBodyCom->Set_Ground(false);
			m_pRigidBodyCom->Set_Gravity(true);


			_vector vLook = -1.f * m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			vLook = XMVectorSetY(vLook, 3.f);
			m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(vLook), 4.5f);
		}	
		else
		{
			if (!m_bFirstChange)
			{
				m_pOwnerMonster->Set_Infinite(0.5f, true);
				m_pModelCom->Set_AnimIndex(m_AnimationIndices[2]);
				m_bFirstChange = true;
			}
				

			m_fAccRecovery += fTimeDelta;
			if (m_fAccRecovery >= m_fRecoveryTime)
			{
				m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
			}
		}
	}
}

void CState_Monster_Damaged_Bound::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_fAccRecovery = 0.f;
	m_bFirstGround = false;

	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
}

CState_Monster_Damaged_Bound* CState_Monster_Damaged_Bound::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Monster_Damaged_Bound* pInstance =  new CState_Monster_Damaged_Bound(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Monster_Damaged_Bound");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Monster_Damaged_Bound::Free()
{
	__super::Free();
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pOwnerMonster);
}

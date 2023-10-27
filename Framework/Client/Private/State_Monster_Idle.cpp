#include "stdafx.h"
#include "State_Monster_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"

CState_Monster_Idle::CState_Monster_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Monster_Idle::Initialize(const list<wstring>& AnimationList)
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

void CState_Monster_Idle::Enter_State(void* pArg)
{
	m_pModelCom->Set_AnimIndex(m_AnimationIndices[0]);
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
}

void CState_Monster_Idle::Tick_State(_float fTimeDelta)
{
	list<CGameObject*> Objects =  GI->Find_GameObjects(LEVEL_GAMEPLAY, LAYER_TYPE::LAYER_CHARACTER);

	for (auto& pGameObject : Objects)
	{
		CTransform* pTargetTransform = pGameObject->Get_Component<CTransform>(L"Com_Transform");

		if (pTargetTransform != nullptr)
		{
			_vector vTargetPos = m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
			_vector vPositon = pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION);

			_vector vDir = vTargetPos - vPositon;
			if (m_fDistance >= XMVectorGetX(XMVector3Length(vDir)))
			{
				m_pStateMachineCom->Change_State(CMonster::TRACE);
				return;
			}
				

			
		}	
	}
}

void CState_Monster_Idle::Exit_State()
{
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
}

CState_Monster_Idle* CState_Monster_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Monster_Idle* pInstance =  new CState_Monster_Idle(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Monster_Idle");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Monster_Idle::Free()
{
	__super::Free();
}

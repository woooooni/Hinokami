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
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;
	
	return S_OK;
}

void CState_Monster_Idle::Enter_State(void* pArg)
{
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pModelCom->Set_AnimIndex(m_AnimIndices[0]);

	m_fAccAttackCoolTime = 0.f;
	m_bAttackable = false;
}

void CState_Monster_Idle::Tick_State(_float fTimeDelta)
{
	m_fAccAttackCoolTime += fTimeDelta;
	if (m_fAccAttackCoolTime >= m_fAttackCoolTime)
	{
		m_fAccAttackCoolTime = 0.f;
		m_bAttackable = true;
	}	

	if (m_bAttackable)
	{
		if (GI->Get_CurrentLevel() == LEVEL_TRAIN)
		{
			m_pStateMachineCom->Change_State(CMonster::DEFENCE_TRACE);
			return;
		}
	}
	
	const list<CGameObject*>& Objects =  GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_CHARACTER);
	for (auto& pGameObject : Objects)
	{
		CTransform* pTargetTransform = pGameObject->Get_Component<CTransform>(L"Com_Transform");
		if (pTargetTransform != nullptr)
		{
			m_pTransformCom->LookAt_ForLandObject(pTargetTransform->Get_State(CTransform::STATE_POSITION));
			

			

			_vector vTargetPos = m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
			_vector vPositon = pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION);

			_vector vDir = vTargetPos - vPositon;

			if (m_bAttackable && XMVectorGetX(XMVector3Length(vDir)) <= 1.2f)
			{
				m_pStateMachineCom->Change_State(CMonster::ATTACK);
				return;
			}

			if (m_bAttackable && (m_fDistance >= XMVectorGetX(XMVector3Length(vDir))))
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

	m_fAccAttackCoolTime = 0.f;
	m_bAttackable = false;
}

CState_Monster_Idle* CState_Monster_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Monster_Idle* pInstance =  new CState_Monster_Idle(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Boss_Idle");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Monster_Idle::Free()
{
	__super::Free();
}

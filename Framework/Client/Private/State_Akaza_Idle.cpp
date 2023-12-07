#include "stdafx.h"
#include "State_Akaza_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"

CState_Akaza_Idle::CState_Akaza_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Akaza_Idle::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;
	
	return S_OK;
}

void CState_Akaza_Idle::Enter_State(void* pArg)
{
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pModelCom->Set_AnimIndex(m_AnimIndices[0]);

	m_fAccAttackCoolTime = 0.f;
	m_bAttackable = false;
}

void CState_Akaza_Idle::Tick_State(_float fTimeDelta)
{
	m_fAccAttackCoolTime += fTimeDelta;
	if (m_fAccAttackCoolTime >= m_fAttackCoolTime)
	{
		m_fAccAttackCoolTime = 0.f;
		m_bAttackable = true;
	}	
	
	const list<CGameObject*>& Objects =  GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_CHARACTER);
	for (auto& pGameObject : Objects)
	{
		CTransform* pTargetTransform = pGameObject->Get_Component<CTransform>(L"Com_Transform");
		if (pTargetTransform != nullptr)
		{
			if (m_bAttackable)
			{
				_float fDistance = XMVectorGetX(XMVector3Length(pTargetTransform->Get_Position() - m_pTransformCom->Get_Position()));
				if(10.f < fDistance)
					m_pStateMachineCom->Change_State(CMonster::BOSS_ATTACK_0);
				else
				{
					_int iRandomAttack = ((rand() + rand() + rand()) % 2) + 1;
					if (iRandomAttack == 1)
					{
						m_pStateMachineCom->Change_State(CMonster::BOSS_ATTACK_1);
					}
					else if (iRandomAttack == 2)
					{
						m_pStateMachineCom->Change_State(CMonster::BOSS_ATTACK_2);
					}
				}
					
				return;
			}
			else
			{
				_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				

				_vector vTargetPos = pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION);
				_vector vPositon = m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);

				_vector vDir = vTargetPos - vPositon;
				_float fDistance = XMVectorGetX(XMVector3Normalize(vDir));

				if (XMConvertToRadians(15.f) > XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), vLook)) 
					&& XMConvertToRadians(-15.f) < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), vLook)))
				{
					m_pTransformCom->LookAt_ForLandObject(vTargetPos);
					m_pStateMachineCom->Change_State(CMonster::DASH);
				}
			}
		}
	}
}

void CState_Akaza_Idle::Exit_State()
{
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

	m_fAccAttackCoolTime = 0.f;
	m_bAttackable = false;
}

CState_Akaza_Idle* CState_Akaza_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Akaza_Idle* pInstance =  new CState_Akaza_Idle(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Akaza_Idle");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Akaza_Idle::Free()
{
	__super::Free();
}

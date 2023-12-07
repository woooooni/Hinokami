#include "stdafx.h"
#include "State_NormalMonster2_Attack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Navigation.h"
#include "Animation.h"
#include "Collider.h"

CState_NormalMonster2_Attack::CState_NormalMonster2_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}


HRESULT CState_NormalMonster2_Attack::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;


	return S_OK;
}

void CState_NormalMonster2_Attack::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = (rand() + rand() + rand()) % m_AnimIndices.size();
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);

	switch (m_iCurrAnimIndex)
	{
	case 0:
		m_pOwner->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
		break;

	case 1:
		m_pOwner->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
		break;

	case 2:
		m_pOwner->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
		break;
	}

	m_pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
}

void CState_NormalMonster2_Attack::Tick_State(_float fTimeDelta)
{
	_float fProgress = m_pModelCom->Get_Animations()[m_pModelCom->Get_CurrAnimationIndex()]->Get_AnimationProgress();
	if (0 == m_iCurrAnimIndex)
	{
		
	}
	else if (1 == m_iCurrAnimIndex)
	{
		if (fProgress >= 0.5f)
		{
			m_pOwner->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 7.f, 0.5f, 1.f);
		}
		

		if (fProgress >= 0.9f)
		{
			m_pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
		}
	}
	else if (2 == m_iCurrAnimIndex)
	{
		if (fProgress >= 0.5f)
		{
			m_pOwner->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 12.f, 1.f, 1.f);
		}

		if (fProgress >= 0.8f)
		{
			m_pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
		}
	}

	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[m_iCurrAnimIndex]))
		m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
}

void CState_NormalMonster2_Attack::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_pOwner->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
}


CState_NormalMonster2_Attack* CState_NormalMonster2_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
	CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_NormalMonster2_Attack* pInstance =  new CState_NormalMonster2_Attack(pDevice, pContext, pStateMachine);

	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_NormalMonster2_Attack");
		return nullptr;
	}
		
	return pInstance;
}

void CState_NormalMonster2_Attack::Free()
{
	__super::Free();
}

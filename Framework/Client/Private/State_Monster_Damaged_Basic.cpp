#include "stdafx.h"
#include "State_Monster_Damaged_Basic.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Animation.h"

CState_Monster_Damaged_Basic::CState_Monster_Damaged_Basic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Monster_Damaged_Basic::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;


	m_pOwnerMonster = dynamic_cast<CMonster*>(m_pOwner);
	if (nullptr == m_pOwnerMonster)
		return E_FAIL;
	
	return S_OK;
}

void CState_Monster_Damaged_Basic::Enter_State(void* pArg)
{
	m_pOwnerMonster->Set_Infinite(0.1f, true);
	m_pOwnerMonster->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

	m_iCurrAnimIndex = (rand() + rand() + rand()) % m_AnimIndices.size();
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
}

void CState_Monster_Damaged_Basic::Tick_State(_float fTimeDelta)
{
	
	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[m_iCurrAnimIndex]))
		m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
		
}

void CState_Monster_Damaged_Basic::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_pOwnerMonster->Set_Infinite(.1f, false);
	m_pOwnerMonster->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
}

CState_Monster_Damaged_Basic* CState_Monster_Damaged_Basic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Monster_Damaged_Basic* pInstance =  new CState_Monster_Damaged_Basic(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Monster_Damaged_Basic");
		return nullptr;
	}

		
	return pInstance;
}

void CState_Monster_Damaged_Basic::Free()
{
	__super::Free();
}

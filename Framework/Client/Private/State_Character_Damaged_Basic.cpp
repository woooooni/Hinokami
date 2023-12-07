#include "stdafx.h"
#include "State_Character_Damaged_Basic.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"


CState_Character_Damaged_Basic::CState_Character_Damaged_Basic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Character_Damaged_Basic::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;

	m_pOwnerCharacter = dynamic_cast<CCharacter*>(m_pOwner);
	if (nullptr == m_pOwnerCharacter)
		return E_FAIL;
	
	return S_OK;
}

void CState_Character_Damaged_Basic::Enter_State(void* pArg)
{

	m_pOwnerCharacter->DrawSword();
	m_pOwnerCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	
	m_iCurrAnimIndex = rand() % m_AnimIndices.size();
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);

}

void CState_Character_Damaged_Basic::Tick_State(_float fTimeDelta)
{
	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[m_iCurrAnimIndex]))
		m_pStateMachineCom->Change_State(CCharacter::STATE::BATTLE_IDLE);
}

void CState_Character_Damaged_Basic::Exit_State()
{
	
}

CState_Character_Damaged_Basic* CState_Character_Damaged_Basic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Character_Damaged_Basic* pInstance =  new CState_Character_Damaged_Basic(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Character_Damaged_Basic");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Character_Damaged_Basic::Free()
{
	__super::Free();
	Safe_Release(m_pOwnerCharacter);
}

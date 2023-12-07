#include "stdafx.h"
#include "State_Character_Dead.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Sword.h"


CState_Character_Dead::CState_Character_Dead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Character_Dead::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;

	m_pCharacter = dynamic_cast<CCharacter*>(m_pStateMachineCom->Get_Owner());
	if (nullptr == m_pCharacter)
		return E_FAIL;

	m_pSword = m_pCharacter->Get_Part<CSword>(CCharacter::PART_SWORD);
	if (nullptr == m_pSword)
		return E_FAIL;
	
	return S_OK;
}

void CState_Character_Dead::Enter_State(void* pArg)
{

	m_pCharacter->DrawSword();


	m_pCharacter->Set_Infinite(999.f, true);
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::BOUNDARY, false);
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::HEAD, false);
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::BODY, false);
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

	m_pModelCom->Set_AnimIndex(m_AnimIndices[0]);
}

void CState_Character_Dead::Tick_State(_float fTimeDelta)
{
	if (KEY_TAP(KEY::R))
	{
		CCharacter::CHARACTER_STAT StatDesc = m_pCharacter->Get_Stat();
		StatDesc.fHp = StatDesc.fMaxHp;

		m_pCharacter->Set_Stat(StatDesc);
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
		m_pCharacter->Set_Infinite(2.f, true);
	}
}

void CState_Character_Dead::Exit_State()
{
	m_pCharacter->Set_Infinite(10.f, true);
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::BOUNDARY, true);
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::HEAD, true);
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::BODY, true);
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
}

CState_Character_Dead* CState_Character_Dead::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Character_Dead* pInstance =  new CState_Character_Dead(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Character_Dead");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Character_Dead::Free()
{
	__super::Free();
}

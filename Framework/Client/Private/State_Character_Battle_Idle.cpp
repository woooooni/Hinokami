#include "stdafx.h"
#include "State_Character_Battle_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Sword.h"
#include "Character.h"

CState_Character_Battle_Idle::CState_Character_Battle_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Character_Battle_Idle::Initialize(const list<wstring>& AnimationList)
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

void CState_Character_Battle_Idle::Enter_State(void* pArg)
{
	if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::D))
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_MOVE);
		return;
	}

	if (CSword::ZENITSU != m_pSword->Get_SwordType())
		m_pCharacter->DrawSword();
	else
		m_pCharacter->SweathSword();

	m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pModelCom->Set_AnimIndex(m_AnimIndices[0]);
	m_fAccBaseNut = 0.f;
}

void CState_Character_Battle_Idle::Tick_State(_float fTimeDelta)
{
	m_fAccBaseNut += fTimeDelta;
	if (m_fAccBaseNut >= m_fBaseNutTime)
		m_pStateMachineCom->Change_State(CCharacter::BASIC_IDLE);



	if (KEY_TAP(KEY::W) || KEY_HOLD(KEY::S) || KEY_TAP(KEY::A) || KEY_HOLD(KEY::D))
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_MOVE);

	if (KEY_TAP(KEY::LBTN))
		m_pStateMachineCom->Change_State(CCharacter::ATTACK);


	if (KEY_TAP(KEY::SPACE))
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_JUMP);

	if(KEY_TAP(KEY::NUM_1))
		m_pStateMachineCom->Change_State(CCharacter::SKILL_0);

	if (KEY_TAP(KEY::NUM_2))
		m_pStateMachineCom->Change_State(CCharacter::SKILL_1);

	if (KEY_TAP(KEY::NUM_3))
		m_pStateMachineCom->Change_State(CCharacter::SKILL_2);
}

void CState_Character_Battle_Idle::Exit_State()
{

}

CState_Character_Battle_Idle* CState_Character_Battle_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Character_Battle_Idle* pInstance =  new CState_Character_Battle_Idle(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Character_Battle_Idle");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Character_Battle_Idle::Free()
{
	__super::Free();
}

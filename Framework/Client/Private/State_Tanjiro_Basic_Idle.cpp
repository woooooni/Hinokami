#include "stdafx.h"
#include "State_Tanjiro_Basic_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Sword.h"

CState_Tanjiro_Basic_Idle::CState_Tanjiro_Basic_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Tanjiro_Basic_Idle::Initialize(const list<wstring>& AnimationList)
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

void CState_Tanjiro_Basic_Idle::Enter_State(void* pArg)
{

	m_pCharacter->SweathSword();
	m_pSword->Stop_Trail();

	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

	m_iCurrAnimIndex = 0;
	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);

	if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::D))
	{
		m_pStateMachineCom->Change_State(CCharacter::BASIC_MOVE);
		return;
	}
}

void CState_Tanjiro_Basic_Idle::Tick_State(_float fTimeDelta)
{

	if (KEY_TAP(KEY::W) || KEY_TAP(KEY::S) || KEY_TAP(KEY::A) || KEY_HOLD(KEY::D))
		m_pStateMachineCom->Change_State(CCharacter::BASIC_MOVE);

	if (KEY_TAP(KEY::LBTN))
		m_pStateMachineCom->Change_State(CCharacter::ATTACK);

	if (KEY_TAP(KEY::SPACE))
		m_pStateMachineCom->Change_State(CCharacter::BASIC_JUMP);
}

void CState_Tanjiro_Basic_Idle::Exit_State()
{
	m_iCurrAnimIndex = 0;
}

CState_Tanjiro_Basic_Idle* CState_Tanjiro_Basic_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Tanjiro_Basic_Idle* pInstance =  new CState_Tanjiro_Basic_Idle(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Tanjiro_Basic_Idle");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Tanjiro_Basic_Idle::Free()
{
	__super::Free();
}

#include "stdafx.h"
#include "State_Tanjiro_Basic_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"

CState_Tanjiro_Basic_Idle::CState_Tanjiro_Basic_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Tanjiro_Basic_Idle::Initialize(const list<wstring>& AnimationList)
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

void CState_Tanjiro_Basic_Idle::Enter_State(void* pArg)
{
	if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::D))
	{
		m_pStateMachineCom->Change_State(CCharacter::BASIC_MOVE);
		return;
	}

	m_pModelCom->Set_AnimIndex(m_AnimationIndices[0]);

	CGameObject* pOwner = m_pStateMachineCom->Get_Owner();
	if (nullptr != pOwner)
	{
		CCharacter* pCharacter = dynamic_cast<CCharacter*>(pOwner);
		if (pCharacter != nullptr)		
			pCharacter->SweathSword();
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

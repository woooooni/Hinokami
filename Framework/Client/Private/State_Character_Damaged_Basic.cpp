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
	m_pModelCom = m_pStateMachineCom->Get_Owner()->Get_Component<CModel>(L"Com_Model");
	if (nullptr == m_pModelCom)
		return E_FAIL;


	m_pTransformCom = m_pStateMachineCom->Get_Owner()->Get_Component<CTransform>(L"Com_Transform");
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_pCharacter = dynamic_cast<CCharacter*>(m_pStateMachineCom->Get_Owner());
	if (nullptr == m_pCharacter)
		return E_FAIL;

	Safe_AddRef(m_pModelCom);
	Safe_AddRef(m_pTransformCom);
	Safe_AddRef(m_pCharacter);

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

void CState_Character_Damaged_Basic::Enter_State(void* pArg)
{
	m_pCharacter->Set_Infinite(5.f, true);
	m_pCharacter->DrawSword();
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	

	m_iRandomIndex = rand() % m_AnimationIndices.size();
	m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iRandomIndex]);

}

void CState_Character_Damaged_Basic::Tick_State(_float fTimeDelta)
{
	if (m_pModelCom->Is_Animation_Finished(m_AnimationIndices[m_iRandomIndex]))
		m_pStateMachineCom->Change_State(CCharacter::STATE::BATTLE_IDLE);
}

void CState_Character_Damaged_Basic::Exit_State()
{
	m_pCharacter->Set_Infinite(0.5f, true);
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
	Safe_Release(m_pCharacter);
}

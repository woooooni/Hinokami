#include "stdafx.h"
#include "State_Character_Dead.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"


CState_Character_Dead::CState_Character_Dead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Character_Dead::Initialize(const list<wstring>& AnimationList)
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

void CState_Character_Dead::Enter_State(void* pArg)
{
	CGameObject* pOwner = m_pStateMachineCom->Get_Owner();
	if (nullptr != pOwner)
	{
		CCharacter* pCharacter = dynamic_cast<CCharacter*>(pOwner);
		if (pCharacter != nullptr)
			pCharacter->DrawSword();

		pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::BOUNDARY, false);
		pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::HEAD, false);
		pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::BODY, false);
		pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	}

	m_pModelCom->Set_AnimIndex(m_AnimationIndices[0]);
}

void CState_Character_Dead::Tick_State(_float fTimeDelta)
{
	
}

void CState_Character_Dead::Exit_State()
{
	CGameObject* pOwner = m_pStateMachineCom->Get_Owner();
	pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::BOUNDARY, true);
	pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::HEAD, true);
	pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::BODY, true);
	pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
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

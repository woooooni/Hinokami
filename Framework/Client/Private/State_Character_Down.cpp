#include "stdafx.h"
#include "State_Character_Down.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Sword.h"
#include "Animation.h"


CState_Character_Down::CState_Character_Down(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Character_Down::Initialize(const list<wstring>& AnimationList)
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

void CState_Character_Down::Enter_State(void* pArg)
{

	m_iCurrAnimIndex = 0;
	m_pCharacter->DrawSword();

	m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::BOUNDARY, false);
	m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::HEAD, false);
	m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::BODY, false);
	m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::BOUNDARY, true);
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::HEAD, true);
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::BODY, true);
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

	m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
}

void CState_Character_Down::Tick_State(_float fTimeDelta)
{
	_float fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();

	switch(m_iCurrAnimIndex)
	{
	case 0:
		if (fProgress >= 0.9f)
			m_pModelCom->Set_AnimIndex(m_AnimIndices[++m_iCurrAnimIndex]);

		break;
	}
}

void CState_Character_Down::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::BOUNDARY, true);
	m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::HEAD, true);
	m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::BODY, true);
	m_pSword->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);

	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::BOUNDARY, true);
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::HEAD, true);
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::BODY, true);
	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
}

CState_Character_Down* CState_Character_Down::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Character_Down* pInstance =  new CState_Character_Down(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Character_Down");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Character_Down::Free()
{
	__super::Free();
}

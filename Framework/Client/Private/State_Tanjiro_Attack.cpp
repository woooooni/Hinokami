#include "stdafx.h"
#include "State_Tanjiro_Attack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"

CState_Tanjiro_Attack::CState_Tanjiro_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTransform* pTransform, CStateMachine* pStateMachine, CModel* pModel)
	: CState(pStateMachine, pModel, pTransform)
{

}

HRESULT CState_Tanjiro_Attack::Initialize(const list<wstring>& AnimationList)
{
	

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

void CState_Tanjiro_Attack::Enter_State(void* pArg)
{
	CGameObject* pOwner = m_pStateMachineCom->Get_Owner();
	if (nullptr != pOwner)
	{
		CCharacter* pCharacter = dynamic_cast<CCharacter*>(pOwner);
		if (pCharacter != nullptr)
			pCharacter->DrawSword();
	}

	m_iCurrAnimIndex = 0;
	m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iCurrAnimIndex]);
	m_fInputDelay = 0.01f;
}

void CState_Tanjiro_Attack::Tick_State(_float fTimeDelta)
{
	if (m_pModelCom->Is_Animation_Finished(m_AnimationIndices[m_iCurrAnimIndex]))
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);

	m_fAccInput += fTimeDelta;
	if (m_fAccInput >= m_fInputDelay)
	{
		if (KEY_TAP(KEY::LBTN))
		{
			if (m_iCurrAnimIndex < m_AnimationIndices.size() - 1)
			{
				if (m_iCurrAnimIndex == 3)
					m_iCurrAnimIndex = 5;
				else
					m_iCurrAnimIndex++;
				
				m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iCurrAnimIndex]);
				m_fInputDelay = m_pModelCom->Get_Animations()[m_AnimationIndices[m_iCurrAnimIndex]]->Get_Duration() / 50.f;
				m_fAccInput = 0.f;
			}
		}

		if (KEY_TAP(KEY::RBTN))
		{
			if (m_iCurrAnimIndex == 2)
			{
				m_iCurrAnimIndex = 4;
				m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iCurrAnimIndex]);
				m_fInputDelay = m_pModelCom->Get_Animations()[m_AnimationIndices[m_iCurrAnimIndex]]->Get_Duration() / 50.f;
				m_fAccInput = 0.f;
			}
		}
	}
}

void CState_Tanjiro_Attack::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_fAccInput = 0.f;
}

CState_Tanjiro_Attack* CState_Tanjiro_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTransform* pTransform, CStateMachine* pStateMachine,
	CModel* pModel,const list<wstring>& AnimationList)
{
	CState_Tanjiro_Attack* pInstance =  new CState_Tanjiro_Attack(pDevice, pContext, pTransform, pStateMachine, pModel);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Tanjiro_Attack");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Tanjiro_Attack::Free()
{
	__super::Free();
}

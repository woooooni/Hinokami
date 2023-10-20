#include "stdafx.h"
#include "State_Tanjiro_Jump.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "RigidBody.h"

CState_Tanjiro_Jump::CState_Tanjiro_Jump(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTransform* pTransform, CStateMachine* pStateMachine, CModel* pModel)
	: CState(pStateMachine, pModel, pTransform)
{

}

HRESULT CState_Tanjiro_Jump::Initialize(const list<wstring>& AnimationList)
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

void CState_Tanjiro_Jump::Enter_State(void* pArg)
{
	if (m_pRigidBody == nullptr)
	{
		CComponent* pComponent = m_pStateMachineCom->Get_Owner()->Get_Component(L"Com_RigidBody");
		if (nullptr == pComponent)
		{
			MSG_BOX("GetRigidBody Failed : CState_Tanjiro_Jump");
			return;
		}


		m_pRigidBody = dynamic_cast<CRigidBody*>(pComponent);
		if (nullptr == m_pRigidBody)
		{
			MSG_BOX("GetRigidBody Failed : CState_Tanjiro_Jump");
			return;
		}
	}


	m_iCurrAnimIndex = 0;
	m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iCurrAnimIndex]);
}

void CState_Tanjiro_Jump::Tick_State(_float fTimeDelta)
{
	if (m_pModelCom->Is_Animation_Finished(m_AnimationIndices[0]))
	{
		m_iCurrAnimIndex++;
		m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iCurrAnimIndex]);
	}

	// if(m_pRigidBody->)
	// TODO :: 점프 구현. m_pRigidBody->Is_Ground()
}

void CState_Tanjiro_Jump::Exit_State()
{

}

CState_Tanjiro_Jump* CState_Tanjiro_Jump::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTransform* pTransform, CStateMachine* pStateMachine,
	CModel* pModel,const list<wstring>& AnimationList)
{
	CState_Tanjiro_Jump* pInstance =  new CState_Tanjiro_Jump(pDevice, pContext, pTransform, pStateMachine, pModel);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Tanjiro_Jump");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Tanjiro_Jump::Free()
{
}

#include "stdafx.h"
#include "State_Tanjiro_Attack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Character.h"
#include "Animation.h"
#include "Sword.h"

CState_Tanjiro_Attack::CState_Tanjiro_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Tanjiro_Attack::Initialize(const list<wstring>& AnimationList)
{
	m_pModelCom = m_pStateMachineCom->Get_Owner()->Get_Component<CModel>(L"Com_Model");
	if (nullptr == m_pModelCom)
		return E_FAIL;


	m_pTransformCom = m_pStateMachineCom->Get_Owner()->Get_Component<CTransform>(L"Com_Transform");
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_pRigidBodyCom = m_pStateMachineCom->Get_Owner()->Get_Component<CRigidBody>(L"Com_RigidBody");
	if (nullptr == m_pRigidBodyCom)
		return E_FAIL;

	m_pCharacter = dynamic_cast<CCharacter*>(m_pStateMachineCom->Get_Owner());
	if (nullptr == m_pCharacter)
		return E_FAIL;

	m_pSword = m_pCharacter->Get_Part<CSword>(CCharacter::PART_SWORD);
	if (nullptr == m_pSword)
		return E_FAIL;

	Safe_AddRef(m_pRigidBodyCom);
	Safe_AddRef(m_pCharacter);
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

void CState_Tanjiro_Attack::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = 0;

	m_pCharacter->DrawSword();
	m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, true);
	m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iCurrAnimIndex]);
}

void CState_Tanjiro_Attack::Tick_State(_float fTimeDelta)
{
	
	Input(fTimeDelta);

	if (m_pModelCom->Is_Animation_Finished(m_AnimationIndices[m_iCurrAnimIndex]))
	{
		m_pStateMachineCom->Change_State(CCharacter::BATTLE_IDLE);
	}
		

	if (m_iCurrAnimIndex == 4)
	{
		_float fAnimationProgress = m_pModelCom->Get_Animations()[m_AnimationIndices[m_iCurrAnimIndex]]->Get_AnimationProgress();
		if (fAnimationProgress >= 0.1f && fAnimationProgress <= 0.5f)
			m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, true);
		else
			m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, false);
	}
}

void CState_Tanjiro_Attack::Exit_State()
{
	m_iCurrAnimIndex = 0;
	
	m_pSword->Set_SwordMode(CSword::SWORD_MODE::BASIC);
	m_pSword->Set_PushPower(0.f);

	m_pCharacter->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
}


void CState_Tanjiro_Attack::Input(_float fTimeDelta)
{
	_float fLookVelocity = 3.f;
	_float fAnimationProgress = m_pModelCom->Get_Animations()[m_AnimationIndices[m_iCurrAnimIndex]]->Get_AnimationProgress();
	if (fAnimationProgress >= 0.5f)
	{
		if (KEY_TAP(KEY::LBTN))
		{
			m_iCurrAnimIndex = min(m_iCurrAnimIndex + 1, m_AnimationIndices.size());
			if (m_iCurrAnimIndex != m_AnimationIndices.size())
				m_pModelCom->Set_AnimIndex(m_AnimationIndices[m_iCurrAnimIndex]);

			switch (m_iCurrAnimIndex)
			{
			case 1:
				m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, true);
				m_pSword->Set_SwordMode(CSword::SWORD_MODE::BASIC);
				m_pSword->Set_PushPower(fLookVelocity);
				m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), fLookVelocity);
				break;

			case 2:
				m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, true);
				m_pSword->Set_SwordMode(CSword::SWORD_MODE::BASIC);
				m_pSword->Set_PushPower(fLookVelocity);
				m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), fLookVelocity);
				break;

			case 3:
				m_pCharacter->Set_ActiveColliders(CCollider::ATTACK, true);
				m_pSword->Set_SwordMode(CSword::SWORD_MODE::BASIC);
				m_pSword->Set_PushPower(fLookVelocity);
				m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), fLookVelocity);
				break;

			case 4:
				m_pSword->Set_SwordMode(CSword::SWORD_MODE::BLOW);
				m_pSword->Set_PushPower(10.f);
				m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), fLookVelocity);
				break;

			default:
				m_pSword->Set_SwordMode(CSword::SWORD_MODE::BASIC);
				m_iCurrAnimIndex = min(m_iCurrAnimIndex + 1, m_AnimationIndices.size() - 1);
				break;
			}
		}
	}
}

CState_Tanjiro_Attack* CState_Tanjiro_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Tanjiro_Attack* pInstance =  new CState_Tanjiro_Attack(pDevice, pContext, pStateMachine);
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
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pCharacter);
}

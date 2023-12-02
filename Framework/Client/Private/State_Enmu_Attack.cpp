#include "stdafx.h"
#include "State_Enmu_Attack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Animation.h"

CState_Enmu_Attack::CState_Enmu_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}


HRESULT CState_Enmu_Attack::Initialize(const vector<list<wstring>>& AnimationList)
{
	list<wstring> Temp;
	if (FAILED(__super::Initialize(Temp)))
		return E_FAIL;



	if (AnimationList.size() < ENMU_ATTACK_TYPE::ATTACK_END)
		return E_FAIL;

	for (_uint i = 0; i < ENMU_ATTACK_TYPE::ATTACK_END; ++i)
	{
		for (auto strAnimation : AnimationList[i])
		{
			_int iAnimationIndex = m_pModelCom->Find_AnimationIndex(strAnimation);
			if (-1 == iAnimationIndex)
				return E_FAIL;

			m_AnimationsIndex[i].push_back(iAnimationIndex);
		}
	}


	m_pOwnerMonster = dynamic_cast<CMonster*>(m_pOwner);
	if (nullptr == m_pOwnerMonster)
		return E_FAIL;

	return S_OK;
}

void CState_Enmu_Attack::Enter_State(void* pArg)
{
	m_iCurrAnimIndex = 0;
	

	m_pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);

	
	if (Find_NearTarget_Distance(GI->Get_TimeDelta(L"Timer_GamePlay")) > 10.f)
		m_iRandomAttackIndex = NEAR_ATTACK_0;
	else
		m_iRandomAttackIndex = (rand() + rand() + rand()) % (ENMU_ATTACK_TYPE::FAR_ATTACK_2 - ENMU_ATTACK_TYPE::FAR_ATTACK_0 + 1) + ENMU_ATTACK_TYPE::FAR_ATTACK_0;




	// Initalize Enter State.

	m_pModelCom->Set_AnimIndex(m_AnimationsIndex[m_iRandomAttackIndex][m_iCurrAnimIndex]);
	if (m_iRandomAttackIndex == NEAR_ATTACK_0)
	{
		(m_pModelCom->Get_Animations()[m_AnimationsIndex[NEAR_ATTACK_0][0]])->Set_AnimationSpeed(1.5f);
	}
	else if (ENMU_ATTACK_TYPE::FAR_ATTACK_0 == m_iRandomAttackIndex)
	{
		_vector vLook = -1.f * XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		float fDirX = XMVectorGetX(vLook);
		fDirX = max(fDirX, -1.f);
		fDirX = min(fDirX, 1.f);

		float fDirZ = XMVectorGetZ(vLook);
		fDirZ = max(fDirZ, -1.f);
		fDirZ = min(fDirZ, 1.f);

		_vector vJumpDir = XMVectorSet(fDirX, 0.8f, fDirZ, 0.f);

		m_pRigidBodyCom->Add_Velocity(vJumpDir, 10.f);
	}



}

void CState_Enmu_Attack::Tick_State(_float fTimeDelta)
{
	switch(m_iRandomAttackIndex)
	{
	case ENMU_ATTACK_TYPE::NEAR_ATTACK_0:
		Tick_Near_Attack_0(fTimeDelta);
		break;

	case ENMU_ATTACK_TYPE::FAR_ATTACK_0:
		Tick_Far_Attack_0(fTimeDelta);
		break;

	case ENMU_ATTACK_TYPE::FAR_ATTACK_1:
		Tick_Far_Attack_1(fTimeDelta);
		break;

	case ENMU_ATTACK_TYPE::FAR_ATTACK_2:
		Tick_Far_Attack_2(fTimeDelta);
		break;
	}

	
}

void CState_Enmu_Attack::Exit_State()
{
	m_iRandomAttackIndex = 0;
	m_iCurrAnimIndex = 0;

	m_pOwnerMonster->Set_Infinite(0.f, false);

	m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 0.f, 1.f);
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	(m_pModelCom->Get_Animations()[m_AnimationsIndex[NEAR_ATTACK_0][0]])->Set_AnimationSpeed(1.f);

}



void CState_Enmu_Attack::Tick_Near_Attack_0(_float fTimeDelta)
{
	
	_float fProgress = m_pModelCom->Get_Animations()[m_AnimationsIndex[m_iRandomAttackIndex][m_iCurrAnimIndex]]->Get_AnimationProgress();
	if (0 == m_iCurrAnimIndex)
	{
		if (Find_NearTarget_Distance(fTimeDelta) <= 1.3f)
		{
			m_iCurrAnimIndex++;
			m_pModelCom->Set_AnimIndex(m_AnimationsIndex[m_iRandomAttackIndex][m_iCurrAnimIndex]);
		}
		else
		{
			Follow_NearTarget(fTimeDelta);
		}
	}
	else if (1 == m_iCurrAnimIndex)
	{
		m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 4.f, 1.f);
		m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
		if (fProgress >= .8f)
		{
			m_iCurrAnimIndex++;
			m_pModelCom->Set_AnimIndex(m_AnimationsIndex[m_iRandomAttackIndex][m_iCurrAnimIndex]);
			return;
		}
	}
	else if (2 == m_iCurrAnimIndex)
	{
		m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 4.f, 0.f, 1.f);
		m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
		if (fProgress >= 1.f)
		{
			m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
			return;
		}
		else
		{
			if (fProgress >= .4f)
			{
				m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BLOW, 0.f, 10.f, 1.f);
				m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
			}
		}
	}
}

void CState_Enmu_Attack::Tick_Far_Attack_0(_float fTimeDelta)
{
	_float fProgress = m_pModelCom->Get_Animations()[m_AnimationsIndex[m_iRandomAttackIndex][m_iCurrAnimIndex]]->Get_AnimationProgress();
	m_pOwner->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BLOW, 3.f, 7.f, 1.f);
	m_pStateMachineCom->Get_Owner()->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
	// TODO::捞棋飘 积己
	if (fProgress >= 0.1f && fProgress <= 0.2f)
	{
		_vector vLook = -1.f * XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		float fDirX = XMVectorGetX(vLook);
		fDirX = max(fDirX, -1.f);
		fDirX = min(fDirX, 1.f);

		float fDirZ = XMVectorGetZ(vLook);
		fDirZ = max(fDirZ, -1.f);
		fDirZ = min(fDirZ, 1.f);


		_vector vJumpDir = XMVectorSet(fDirX, 0.8f, fDirZ, 0.f);
		m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(vJumpDir), 5.f);
	}

	if (fProgress >= 0.4f && fProgress <= 0.42f)
	{
		for (_uint i = 0; i < 3; ++i)
		{
			_int iRandom = rand() % 3 - 1;
			Shoot(Find_NearTarget(fTimeDelta), XMVectorSet(_float(iRandom), 0.f, 0.f, 0.f), fTimeDelta);
		}
	}

	if (fProgress >= 1.f)
	{
		
		m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
		return;
	}


}

void CState_Enmu_Attack::Tick_Far_Attack_1(_float fTimeDelta)
{
	// TODO :: 捞棋飘 积己.
	_float fProgress = m_pModelCom->Get_Animations()[m_AnimationsIndex[m_iRandomAttackIndex][m_iCurrAnimIndex]]->Get_AnimationProgress();

	if (m_iCurrAnimIndex == 0)
	{
		if (fProgress >= .3f && fProgress <= .32f)
		{
			_int iRandom = (rand() + rand() + rand()) % 3 - 1;
			Shoot(Find_NearTarget(fTimeDelta), XMVectorSet(_float(iRandom), 0.f, 0.f, 0.f), fTimeDelta);
		}
		if (fProgress >= .4f && fProgress <= .42f)
		{
			_int iRandom = (rand() + rand() + rand()) % 3 - 1;
			Shoot(Find_NearTarget(fTimeDelta), XMVectorSet(_float(iRandom), 0.f, 0.f, 0.f), fTimeDelta);
		}

		if (fProgress >= .5f && fProgress <= .52f)
		{
			_int iRandom = (rand() + rand() + rand()) % 3 - 1;
			Shoot(Find_NearTarget(fTimeDelta), XMVectorSet(_float(iRandom), 0.f, 0.f, 0.f), fTimeDelta);
		}

		if (fProgress >= .6f && fProgress <= .62f)
		{
			_int iRandom = (rand() + rand() + rand()) % 3 - 1;
			Shoot(Find_NearTarget(fTimeDelta), XMVectorSet(_float(iRandom), 0.f, 0.f, 0.f), fTimeDelta);
		}
		if (fProgress >= .8f)
		{
			m_iCurrAnimIndex++;
			m_pModelCom->Set_AnimIndex(m_AnimationsIndex[m_iRandomAttackIndex][m_iCurrAnimIndex]);
			return;
		}
	}

	else if (m_iCurrAnimIndex == 1)
	{
		if (fProgress >= 1.f)
		{
			m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
			return;
		}
	}
}

void CState_Enmu_Attack::Tick_Far_Attack_2(_float fTimeDelta)
{
	_float fProgress = m_pModelCom->Get_Animations()[m_AnimationsIndex[m_iRandomAttackIndex][m_iCurrAnimIndex]]->Get_AnimationProgress();
	m_pOwnerMonster->Set_Infinite(999.f, true);

	if (m_iCurrAnimIndex == 0)
	{
		if (fProgress >= .8f)
		{
			m_iCurrAnimIndex++;
			m_pModelCom->Set_AnimIndex(m_AnimationsIndex[m_iRandomAttackIndex][m_iCurrAnimIndex]);
			return;
		}
	}

	if (m_iCurrAnimIndex == 1)
	{
		if (fProgress >= .8f)
		{
			m_iCurrAnimIndex++;
			m_pModelCom->Set_AnimIndex(m_AnimationsIndex[m_iRandomAttackIndex][m_iCurrAnimIndex]);
			return;
		}
	}

	if (m_iCurrAnimIndex == 2)
	{
		if (fProgress >= .8f)
		{
			m_iCurrAnimIndex++;
			m_pModelCom->Set_AnimIndex(m_AnimationsIndex[m_iRandomAttackIndex][m_iCurrAnimIndex]);
			return;
		}
	}

	if (m_iCurrAnimIndex == 3)
	{
		if (fProgress >= .8f)
		{
			m_iCurrAnimIndex++;
			m_pModelCom->Set_AnimIndex(m_AnimationsIndex[m_iRandomAttackIndex][m_iCurrAnimIndex]);
			return;
		}
	}

	if (m_iCurrAnimIndex == 4)
	{
		if (fProgress >= 1.f)
		{
			m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
			return;
		}

		if (fProgress >= 0.1f && fProgress <= 0.2f)
		{
			_vector vLook = -1.f * XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

			float fDirX = XMVectorGetX(vLook);
			fDirX = max(fDirX, -1.f);
			fDirX = min(fDirX, 1.f);

			float fDirZ = XMVectorGetZ(vLook);
			fDirZ = max(fDirZ, -1.f);
			fDirZ = min(fDirZ, 1.f);


			_vector vJumpDir = XMVectorSet(fDirX, 0.8f, fDirZ, 0.f);
			m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(vJumpDir), 5.f);
		}
	}
}


void CState_Enmu_Attack::Shoot(CGameObject* pTarget, _vector vOffsetPosition, _float fTimeDelta)
{

	CGameObject* pProjectile = nullptr;
	if (FAILED(GI->Add_GameObject(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Enmu_Projectile", nullptr, &pProjectile)))
		return;
	if (FAILED(nullptr == pProjectile))
		return;

	CTransform* pProjectileTransform = pProjectile->Get_Component<CTransform>(L"Com_Transform");
	CTransform* pTargetTransform = pTarget->Get_Component<CTransform>(L"Com_Transform");

	if (nullptr == pProjectileTransform || nullptr == pTargetTransform)
		return;

	_vector vPosition = m_pTransformCom->Get_Position();
	_vector vDir = XMVector3Normalize(pTargetTransform->Get_Position() - m_pTransformCom->Get_Position());
	
	pProjectileTransform->Set_State(CTransform::STATE_LOOK, vDir);
	pProjectileTransform->Set_State(CTransform::STATE_RIGHT, XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), pProjectileTransform->Get_Look()));
	pProjectileTransform->Set_State(CTransform::STATE_UP, XMVector3Cross(pProjectileTransform->Get_Look(), pProjectileTransform->Get_Right()));
	pProjectileTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMVectorSetY(vPosition, XMVectorGetY(vPosition) + 1.f) + vOffsetPosition, 1.f));
}

CGameObject* CState_Enmu_Attack::Find_NearTarget(_float fTimeDelta)
{
	list<CGameObject*>& TargetObjects = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_CHARACTER);
	for (auto& pTarget : TargetObjects)
	{
		CTransform* pTargetTransform = pTarget->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr == pTargetTransform)
			continue;

		return pTarget;
	}
	return nullptr;
}

_float CState_Enmu_Attack::Find_NearTarget_Distance(_float fTimeDelta)
{
	list<CGameObject*>& TargetObjects = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_CHARACTER);

	_float fMinDistance = 999999999.f;
	for (auto& pTarget : TargetObjects)
	{
		CTransform* pTargetTransform = pTarget->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr == pTargetTransform)
			continue;

		_vector vDir = XMVectorSetW(pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f);
		_float fDistance = XMVectorGetX(XMVector3Length(vDir));

		if (fDistance <= fMinDistance)
		{
			fMinDistance = fDistance;
			m_pTransformCom->LookAt_ForLandObject(pTargetTransform->Get_State(CTransform::STATE_POSITION));
		}

	}
	return fMinDistance;
}

void CState_Enmu_Attack::Follow_NearTarget(_float fTimeDelta)
{
	list<CGameObject*>& TargetObjects = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_CHARACTER);

	_float fMinDistance = 999999999.f;
	for (auto& pTarget : TargetObjects)
	{
		CTransform* pTargetTransform = pTarget->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr == pTargetTransform)
			continue;

		_vector vDir = XMVectorSetW(pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f);
		_float fDistance = XMVectorGetX(XMVector3Length(vDir));

		if (fDistance <= fMinDistance)
		{
			fMinDistance = fDistance;
			m_pTransformCom->LookAt_ForLandObject(pTargetTransform->Get_State(CTransform::STATE_POSITION));
			vDir = XMVector3Normalize(XMVectorSetY(vDir, 0.f));
			m_pTransformCom->Go_Dir(vDir, 15.f, fTimeDelta, m_pNavigationCom);
		}
	}
}

CState_Enmu_Attack* CState_Enmu_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	CStateMachine* pStateMachine, const vector<list<wstring>>& AnimationList)
{
	CState_Enmu_Attack* pInstance =  new CState_Enmu_Attack(pDevice, pContext, pStateMachine);

	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Akaza_Attack");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Enmu_Attack::Free()
{
	__super::Free();
}

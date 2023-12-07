#include "stdafx.h"
#include "State_Akaza_Attack_2.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Animation.h"
#include "Utils.h"

CState_Akaza_Attack_2::CState_Akaza_Attack_2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}


HRESULT CState_Akaza_Attack_2::Initialize(const list<wstring>& AnimationList)
{
	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;

	m_pOwnerMonster = dynamic_cast<CMonster*>(m_pOwner);
	if (nullptr == m_pOwnerMonster)
		return E_FAIL;

	return S_OK;
}

void CState_Akaza_Attack_2::Enter_State(void* pArg)
{
	m_fAccShoot = 0.f;
	m_iCurrAnimIndex = 0;
	m_pOwnerMonster->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);
	m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 4.f, 1.f);
	Find_Near_Target();

	_vector vDir = -1.f * m_pTransformCom->Get_Look();
	m_pTransformCom->Set_Position(m_pTransformCom->Get_Position() + XMVectorSet(0.f, 0.1f, 0.f, 0.f), GI->Get_TimeDelta(L"Timer_GamePlay"), m_pNavigationCom);
	m_pRigidBodyCom->Add_Velocity(XMVector3Normalize(XMVectorSetY(vDir, 0.8f)), 14.f);

	GI->Play_Sound(L"Voice_Akaza_Skill_0.wav", CHANNELID::SOUND_VOICE_MONSTER2, 1.f, true);
}

void CState_Akaza_Attack_2::Tick_State(_float fTimeDelta)
{
	_float fProgress = m_pModelCom->Get_CurrAnimation()->Get_AnimationProgress();
	switch (m_iCurrAnimIndex)
	{
	case 0:
		if (fProgress >= .9f)
		{
			++m_iCurrAnimIndex;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
		}
		break;

	case 1:
	{
		m_fAccShoot += fTimeDelta;
		if (m_fAccShoot >= m_fShootTime)
		{
			m_fAccShoot = 0.f;
			Shoot(fTimeDelta);
		}
		

		_float3 vVelocity = m_pRigidBodyCom->Get_Velocity();
		_float fForce = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vVelocity)));

		if (fForce <= 0.1f)
		{
			++m_iCurrAnimIndex;
			m_pModelCom->Set_AnimIndex(m_AnimIndices[m_iCurrAnimIndex]);
		}
	}
		
		break;

	case 2:
		if (fProgress >= .9f)
		{
			m_pStateMachineCom->Change_State(CMonster::IDLE);
			return;
		}
		break;
	}

}

void CState_Akaza_Attack_2::Exit_State()
{
	m_iCurrAnimIndex = 0;
	m_fAccShoot = 0.f;
	m_pOwnerMonster->Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 4.f, 1.f);
	m_pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);

}







void CState_Akaza_Attack_2::Find_Near_Target()
{
	_float fMinDistance = 9999999999.f;
	CTransform* pFindTargetTransform = nullptr;

	m_pTarget = nullptr;
	const list<CGameObject*>& Targets = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_CHARACTER);
	for (auto& pTarget : Targets)
	{
		CTransform* pTargetTransform = pTarget->Get_Component<CTransform>(L"Com_Transform");

		if (nullptr == pTargetTransform)
			continue;

		_float fDistance = Vec3::Distance(pTargetTransform->Get_Position(), m_pTransformCom->Get_Position());
		if (fDistance < fMinDistance)
		{
			fMinDistance = fDistance;
			pFindTargetTransform = pTargetTransform;
			m_pTarget = pTarget;
		}
	}

	if (nullptr != pFindTargetTransform && fMinDistance < 20.f)
	{
		Vec4 vTargetPos = pFindTargetTransform->Get_Position();
		m_pTransformCom->LookAt_ForLandObject(vTargetPos);
	}
}

void CState_Akaza_Attack_2::Shoot(_float fTimeDelta)
{
	Find_Near_Target();

	CGameObject* pProjectile = nullptr;
	if (FAILED(GI->Add_GameObject(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Akaza_Projectile", nullptr, &pProjectile)))
		return;

	if (FAILED(nullptr == pProjectile))
		return;

	CTransform* pProjectileTransform = pProjectile->Get_Component<CTransform>(L"Com_Transform");
	CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>(L"Com_Transform");

	if (nullptr == pProjectileTransform || nullptr == pTargetTransform)
		return;

	_vector vPosition = m_pTransformCom->Get_Position();
	_vector vDir = XMVector3Normalize(pTargetTransform->Get_Position() - m_pTransformCom->Get_Position());

	_float fRandomX = CUtils::Random_Float(-2.f, 2.f);
	pProjectileTransform->Set_State(CTransform::STATE_LOOK, vDir);
	pProjectileTransform->Set_State(CTransform::STATE_RIGHT, XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), pProjectileTransform->Get_Look()));
	pProjectileTransform->Set_State(CTransform::STATE_UP, XMVector3Cross(pProjectileTransform->Get_Look(), pProjectileTransform->Get_Right()));
	pProjectileTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMVectorSetY(vPosition, XMVectorGetY(vPosition) + 1.f) + XMVectorSet(fRandomX, 0.f, 0.f, 0.f), 1.f));
}

CState_Akaza_Attack_2* CState_Akaza_Attack_2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	CStateMachine* pStateMachine, const list<wstring>& AnimationList)
{
	CState_Akaza_Attack_2* pInstance = new CState_Akaza_Attack_2(pDevice, pContext, pStateMachine);

	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Akaza_Attack_2");
		return nullptr;
	}

	return pInstance;
}

void CState_Akaza_Attack_2::Free()
{
	__super::Free();
}

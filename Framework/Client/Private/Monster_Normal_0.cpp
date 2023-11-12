#include "stdafx.h"
#include "Monster_Normal_0.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Sword.h"
#include "Sweath.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"


#include "State_Monster_Attack.h"
#include "State_Monster_Damaged_Basic.h"
#include "State_Monster_Damaged_AirBorn.h"
#include "State_Monster_Damaged_Blow.h"
#include "State_Monster_Idle.h"
#include "State_Monster_Jump.h"
#include "State_Monster_Trace.h"
#include "State_Monster_Regen.h"
#include "State_Monter_Dead.h"

USING(Client)
CMonster_Normal_0::CMonster_Normal_0(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const MONSTER_STAT& tStat)
	: CMonster(pDevice, pContext, strObjectTag, tStat)
{
}

CMonster_Normal_0::CMonster_Normal_0(const CMonster_Normal_0& rhs)
	: CMonster(rhs)
{

}

HRESULT CMonster_Normal_0::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CMonster_Normal_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Sockets()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

 	if (FAILED(Ready_Colliders()))
		return E_FAIL;

	return S_OK;
}

void CMonster_Normal_0::Tick(_float fTimeDelta)
{
	m_pStateCom->Tick_State(fTimeDelta);
	m_pRigidBodyCom->Tick_RigidBody(fTimeDelta);
	__super::Tick(fTimeDelta);
}

void CMonster_Normal_0::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
	GI->Add_CollisionGroup(COLLISION_GROUP::MONSTER, this);
}


void CMonster_Normal_0::Collision_Enter(const COLLISION_INFO& tInfo)
{

}

void CMonster_Normal_0::Collision_Continue(const COLLISION_INFO& tInfo)
{
	__super::Collision_Continue(tInfo);
}

void CMonster_Normal_0::Collision_Exit(const COLLISION_INFO& tInfo)
{
	__super::Collision_Exit(tInfo);
}

void CMonster_Normal_0::On_Damaged(CGameObject* pAttacker, _uint eColliderDamageType, _float fDamage)
{
	if (m_bInfinite || m_bReserveDead)
		return;

	__super::On_Damaged(pAttacker, eColliderDamageType, fDamage);
	CTransform* pAttackerTransform = pAttacker->Get_Component<CTransform>(L"Com_Transform");

	m_pTransformCom->LookAt_ForLandObject(pAttackerTransform->Get_State(CTransform::STATE_POSITION));

	m_tStat.fHp -= fDamage;
	if (m_tStat.fHp <= 0.f)
	{
		m_bReserveDead = true;
		m_pStateCom->Change_State(MONSTER_STATE::DIE);
		return;
	}

	switch (eColliderDamageType)
	{
	case CCollider::ATTACK_TYPE::BASIC:
		m_pStateCom->Change_State(MONSTER_STATE::DAMAGED_BASIC);
		break;

	case CCollider::ATTACK_TYPE::AIR_BORN:
		m_pStateCom->Change_State(MONSTER_STATE::DAMAGED_AIRBORN);
		_vector vPosition = m_pTransformCom->Get_WorldMatrix().r[CTransform::STATE_POSITION];
		Set_Infinite(0.5f, true);
		break;

	case CCollider::ATTACK_TYPE::BLOW:
		m_pStateCom->Change_State(MONSTER_STATE::DAMAGED_BLOW);
		m_pRigidBodyCom->Add_Velocity(XMVectorSetY(XMVector3Normalize(-1.f * m_pTransformCom->Get_State(CTransform::STATE_LOOK)), 0.f), 10.f);
		Set_Infinite(0.5f, true);
		break;

	case CCollider::ATTACK_TYPE::BOUND:
		m_pStateCom->Change_State(MONSTER_STATE::DAMAGED_BOUND);
		Set_Infinite(0.5f, true);
		break;
	}

}


HRESULT CMonster_Normal_0::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_NormalMonster0"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_StateMachine */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateCom)))
		return E_FAIL;

	/* For.Com_DissolveTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissoveTexture)))
		return E_FAIL;


	CNavigation::NAVIGATION_DESC NavigationDesc;
	ZeroMemory(&NavigationDesc, sizeof NavigationDesc);

	XMStoreFloat3(&NavigationDesc.vStartWorldPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	NavigationDesc.bInitialize_Index = true;

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
		return E_FAIL;

	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_pNavigationCom->Get_NaviDesc().vStartWorldPosition), 1.f));

	CRigidBody::RIGID_BODY_DESC RigidDesc;
	ZeroMemory(&RigidDesc, sizeof RigidDesc);


	RigidDesc.pNavigation = m_pNavigationCom;
	RigidDesc.pTransform = m_pTransformCom;

	/* For.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigidBodyCom, &RigidDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CMonster_Normal_0::Ready_States()
{
	list<wstring> strAnimationName;

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_E0001_V00_C00_BaseNut01_1");
	m_pStateCom->Add_State(CMonster::MONSTER_STATE::IDLE, CState_Monster_Idle::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_E0001_V00_C00_BaseRun01_1");
	m_pStateCom->Add_State(CMonster::MONSTER_STATE::TRACE, CState_Monster_Trace::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));


	strAnimationName.clear();
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_E0001_V00_C00_BaseJump01_0");
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_E0001_V00_C00_BaseJump01_1");
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_E0001_V00_C00_BaseJump01_2");
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_E0001_V00_C00_BaseJump01_3");
	
	m_pStateCom->Add_State(CMonster::MONSTER_STATE::JUMP, CState_Monster_Jump::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_E0001_V00_C00_AtkBurst01");
	m_pStateCom->Add_State(CMonster::MONSTER_STATE::REGEN, CState_Monster_Regen::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));

	strAnimationName.clear();
	
    
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_E0001_V00_C00_AtkCmbW01");
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_E0001_V00_C00_AtkCmbW02");
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_E0001_V00_C00_AtkSkl01");
	m_pStateCom->Add_State(CMonster::ATTACK, CState_Monster_Attack::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));


	strAnimationName.clear();
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_P0000_V00_C00_Dmg01_F");
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_P0000_V00_C00_Dmg01_L");
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_P0000_V00_C00_Dmg01_R");

	m_pStateCom->Add_State(CMonster::DAMAGED_BASIC, CState_Monster_Damaged_Basic::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_P0000_V00_C00_DmgBlowF01_0");
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_P0000_V00_C00_DmgBlowF01_1");
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_P0000_V00_C00_DmgBlowF01_2");
	m_pStateCom->Add_State(CMonster::DAMAGED_AIRBORN, CState_Monster_Damaged_AirBorn::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));


	


	strAnimationName.clear();
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_P0000_V00_C00_DmgStrikeF01_0");
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_P0000_V00_C00_DmgStrikeF01_1");
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_P0000_V00_C00_DmgDown01_1");
	m_pStateCom->Add_State(CMonster::DAMAGED_BLOW, CState_Monster_Damaged_Blow::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));
	

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_E0001_V00_C00.ao|A_P0000_V00_C00_DmgDown01_1");
	m_pStateCom->Add_State(CMonster::DIE, CState_Monster_Dead::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));


	m_pStateCom->Change_State(CMonster::MONSTER_STATE::REGEN);

	return S_OK;
}

HRESULT CMonster_Normal_0::Ready_Colliders()
{

	CCollider_Sphere::SPHERE_COLLIDER_DESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	BoundingSphere tSphere;
	ZeroMemory(&tSphere, sizeof(BoundingSphere));
	tSphere.Radius = 1.f;
	XMStoreFloat4x4(&ColliderDesc.ModePivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColliderDesc.pOwnerTransform = m_pTransformCom;

	ColliderDesc.tSphere = tSphere;
	ColliderDesc.pNode = m_pModelCom->Get_HierarchyNode(L"Root");
	ColliderDesc.vOffsetPosition = _float3(0.f, 1.f, 0.f);
	
	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::BOUNDARY, &ColliderDesc)))
		return E_FAIL;


	ColliderDesc.tSphere.Radius = .2f;
	ColliderDesc.pNode = m_pModelCom->Get_HierarchyNode(L"C_TongueA_2");
	ColliderDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::HEAD, &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.tSphere.Radius = .6f;
	ColliderDesc.pNode = m_pModelCom->Get_HierarchyNode(L"Root");
	ColliderDesc.vOffsetPosition = _float3(0.f, 0.5f, 0.f);
	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::BODY, &ColliderDesc)))
		return E_FAIL;




	ColliderDesc.tSphere.Radius = 0.3f;
	ColliderDesc.pNode = m_pModelCom->Get_HierarchyNode(L"L_Hand_1_E0001_V00_C00_Lct");
	ColliderDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::ATTACK, &ColliderDesc)))
		return E_FAIL;


	ColliderDesc.pNode = m_pModelCom->Get_HierarchyNode(L"R_Hand_1_E0001_V00_C00_Lct");
	ColliderDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::ATTACK, &ColliderDesc)))
		return E_FAIL;


	ColliderDesc.pNode = m_pModelCom->Get_HierarchyNode(L"L_Foot_End");
	ColliderDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::ATTACK, &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.pNode = m_pModelCom->Get_HierarchyNode(L"R_Foot_End");
	ColliderDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::ATTACK, &ColliderDesc)))
		return E_FAIL;



	// Weapon



	return S_OK;
}

HRESULT CMonster_Normal_0::Ready_Sockets()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	m_Sockets.resize(SOCKET_TYPE::SOCKET_END);

	m_Sockets[SOCKET_TYPE::SOCKET_LEFT_FIST] = m_pModelCom->Get_HierarchyNode(L"R_HandCommon_1_Lct");
	m_Sockets[SOCKET_TYPE::SOCKET_LEFT_FOOT] = m_pModelCom->Get_HierarchyNode(L"R_Hand_1");
	m_Sockets[SOCKET_TYPE::SOCKET_LEFT_FIST] = m_pModelCom->Get_HierarchyNode(L"L_Weapon_1_Lct");
	m_Sockets[SOCKET_TYPE::SOCKET_RIGHT_FIST] = m_pModelCom->Get_HierarchyNode(L"L_Weapon_1_Lct");

	return S_OK;
}

HRESULT CMonster_Normal_0::Ready_Parts()
{
	return S_OK;
}


CMonster_Normal_0* CMonster_Normal_0::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const MONSTER_STAT& tStat)
{
	CMonster_Normal_0* pInstance = new CMonster_Normal_0(pDevice, pContext, strObjectTag, tStat);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CMonster_Normal_0");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CMonster_Normal_0::Clone(void* pArg)
{
	CMonster_Normal_0* pInstance = new CMonster_Normal_0(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_Normal_0");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Normal_0::Free()
{
	__super::Free();
}


#include "stdafx.h"
#include "Boss_Akaza.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Sword.h"
#include "Sweath.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"
#include "Trail.h"




#include "State_Akaza_Idle.h"
#include "State_Monster_Jump.h"
#include "State_Monter_Dead.h"

#include "State_Akaza_Attack_0.h"
#include "State_Akaza_Attack_1.h"
#include "State_Akaza_Attack_2.h"
#include "State_Akaza_Skill_0.h"

#include "State_Boss_Battle_Dash.h"

#include "State_Monster_Damaged_Basic.h"
#include "State_Monster_Damaged_AirBorn.h"
#include "State_Monster_Damaged_Blow.h"
#include "State_Monster_Damaged_Bound.h"


USING(Client)
CBoss_Akaza::CBoss_Akaza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const MONSTER_STAT& tStat)
	: CMonster(pDevice, pContext, strObjectTag, tStat)
{
}

CBoss_Akaza::CBoss_Akaza(const CBoss_Akaza& rhs)
	: CMonster(rhs)
{
}

HRESULT CBoss_Akaza::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CBoss_Akaza::Initialize(void* pArg)
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

	m_eMonsterType = CMonster::AKAZA;

	return S_OK;
}

void CBoss_Akaza::Tick(_float fTimeDelta)
{
	m_pStateCom->Tick_State(fTimeDelta);
	__super::Tick(fTimeDelta);


	if (CMonster::IDLE == m_pStateCom->Get_CurrState() && m_iSkillCount > 0)
	{
		_float fRatio = m_tStat.fHp / m_tStat.fMaxHp;
		if (fRatio <= .8f && m_iSkillCount == 3)
		{
			--m_iSkillCount;
			m_pStateCom->Change_State(MONSTER_STATE::SKILL_0);
		}
		else if (fRatio <= .5f && m_iSkillCount == 2)
		{
			--m_iSkillCount;
			m_pStateCom->Change_State(MONSTER_STATE::SKILL_0);
		}
		else if (fRatio <= .3f && m_iSkillCount == 1)
		{
			--m_iSkillCount;
			m_pStateCom->Change_State(MONSTER_STATE::SKILL_0);
		}
	}


	for (_uint i = 0; i < CMonster::SOCKET_END; ++i)
	{
		if (nullptr == m_pTrails[i])
			continue;
		_matrix		WorldMatrix = m_Sockets[i]->Get_CombinedTransformation() * m_pModelCom->Get_PivotMatrix();
		WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
		WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
		WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

		m_pTrails[i]->Set_TransformMatrix(WorldMatrix * m_pTransformCom->Get_WorldMatrix());
		m_pTrails[i]->Tick(fTimeDelta);
	}
}

void CBoss_Akaza::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
	m_pRigidBodyCom->LateTick_RigidBody(fTimeDelta);

	for (_uint i = 0; i < CMonster::SOCKET_END; ++i)
	{
		if (nullptr == m_pTrails[i])
			continue;

		m_pTrails[i]->LateTick(fTimeDelta);
	}
}

HRESULT CBoss_Akaza::Render()
{
	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &GI->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TransPose(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;


	_float4 vRimColor = { 0.f, 0.f, 0.f, 0.f };
	if (m_bInfinite)
		vRimColor = { 1.f, 0.f, 0.f, 1.f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimColor", &vRimColor, sizeof(_float4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();


	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint		iPassIndex = 0;
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			iPassIndex = 0;
		else
			iPassIndex++;

		if (true == m_bReserveDead)
		{
			iPassIndex = 2;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveWeight", &m_fDissolveWeight, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pDissoveTexture->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
				return E_FAIL;
		}
		else
			iPassIndex = 3;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;

	return S_OK;
}

void CBoss_Akaza::Enter_Scene()
{
}

void CBoss_Akaza::Return_Pool()
{
	
}

void CBoss_Akaza::Collision_Enter(const COLLISION_INFO& tInfo)
{
	__super::Collision_Enter(tInfo);
}

void CBoss_Akaza::Collision_Continue(const COLLISION_INFO& tInfo)
{
	__super::Collision_Continue(tInfo);
}

void CBoss_Akaza::Collision_Exit(const COLLISION_INFO& tInfo)
{
	__super::Collision_Exit(tInfo);
}

void CBoss_Akaza::On_Damaged(const COLLISION_INFO& tInfo)
{
	__super::On_Damaged(tInfo);
	// GI->Set_Slow(L"Timer_GamePlay", 0.1f, 0.f);
}



HRESULT CBoss_Akaza::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Akaza"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_StateMachine */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateCom)))
		return E_FAIL;

	/* For.Com_DissolveTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissoveTexture)))
		return E_FAIL;


	CNavigation::NAVIGATION_DESC NavigationDesc;
	ZeroMemory(&NavigationDesc, sizeof NavigationDesc);

	XMStoreFloat4(&NavigationDesc.vStartWorldPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	NavigationDesc.bInitialize_Index = true;

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
		return E_FAIL;

	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pNavigationCom->Get_NaviDesc().vStartWorldPosition));

	CRigidBody::RIGID_BODY_DESC RigidDesc;
	ZeroMemory(&RigidDesc, sizeof RigidDesc);


	RigidDesc.pNavigation = m_pNavigationCom;
	RigidDesc.pTransform = m_pTransformCom;

	/* For.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigidBodyCom, &RigidDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CBoss_Akaza::Ready_States()
{
	list<wstring> strAnimationName;

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_BaseNut01_1");
	m_pStateCom->Add_State(CMonster::MONSTER_STATE::IDLE, CState_Akaza_Idle::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));


	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_BaseJump01_0");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_BaseJump01_1");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_BaseJump01_2");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_BaseJump01_3");
	
	m_pStateCom->Add_State(CMonster::MONSTER_STATE::JUMP, CState_Monster_Jump::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));


	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_BaseStepL01");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_BaseStepL02");

	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_BaseStepR01");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_BaseStepR02");
	m_pStateCom->Add_State(CMonster::DASH, CState_Boss_Battle_Dash::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));



	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P0000_V00_C00_Dmg02_F");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P0000_V00_C00_Dmg02_G");
	m_pStateCom->Add_State(CMonster::DAMAGED_BASIC, CState_Monster_Damaged_Basic::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));



	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P0000_V00_C00_DmgBlowF01_0");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P0000_V00_C00_DmgBlowF01_1");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P0000_V00_C00_DmgBlowF01_2");
	m_pStateCom->Add_State(CMonster::DAMAGED_AIRBORN, CState_Monster_Damaged_AirBorn::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));



	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_BaseRun01_1");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkSkl03_0");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkSkl03_1");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkSkl03_2");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkSkl03_3");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkSkl03_4");
	m_pStateCom->Add_State(CMonster::BOSS_ATTACK_0, CState_Akaza_Attack_0::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkCmbW01");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkCmbW02");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkCmbW03");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkCmbW03D01");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkCmbW03U01");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkCmbW04");
	
	m_pStateCom->Add_State(CMonster::BOSS_ATTACK_1, CState_Akaza_Attack_1::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkSkl02A_0");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkSkl02A_1");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkSkl02A_2");
	
	m_pStateCom->Add_State(CMonster::BOSS_ATTACK_2, CState_Akaza_Attack_2::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P0000_V00_C00_DmgStrikeF01_0");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P0000_V00_C00_DmgStrikeF01_1");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P0000_V00_C00_DmgDown01_1");
	m_pStateCom->Add_State(CMonster::DAMAGED_BLOW, CState_Monster_Damaged_Blow::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));

	

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkUniqueAct01");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkSkl04_0");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkSkl04_1");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P1012_V00_C00_AtkSkl04_2");
	m_pStateCom->Add_State(CMonster::SKILL_0, CState_Akaza_Skill_0::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P0000_V00_C00_DmgBound01_0");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P0000_V00_C00_DmgBound02_2");
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P0000_V00_C00_DmgDown01_1");
	m_pStateCom->Add_State(CMonster::DAMAGED_BOUND, CState_Monster_Damaged_Bound::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));
	

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1012_V01_C00.ao|A_P0000_V00_C00_DmgBound02_2");
	m_pStateCom->Add_State(CMonster::DIE, CState_Monster_Dead::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));


	m_pStateCom->Change_State(CMonster::MONSTER_STATE::IDLE);

	return S_OK;
}

HRESULT CBoss_Akaza::Ready_Colliders()
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

	ColliderDesc.tSphere.Radius = .6f;
	ColliderDesc.pNode = m_pModelCom->Get_HierarchyNode(L"Root");
	ColliderDesc.vOffsetPosition = _float3(0.f, 0.5f, 0.f);
	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::BODY, &ColliderDesc)))
		return E_FAIL;




	ColliderDesc.tSphere.Radius = 0.3f;
	ColliderDesc.pNode = m_pModelCom->Get_HierarchyNode(L"R_Hand_1_Lct");
	ColliderDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::ATTACK, &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.tSphere.Radius = 0.3f;
	ColliderDesc.pNode = m_pModelCom->Get_HierarchyNode(L"L_Hand_1_Lct");
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



	return S_OK;
}

HRESULT CBoss_Akaza::Ready_Sockets()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	m_Sockets.resize(SOCKET_TYPE::SOCKET_END);

	m_Sockets[SOCKET_TYPE::SOCKET_LEFT_FIST] = m_pModelCom->Get_HierarchyNode(L"L_Hand_1_Lct"); 
	m_Sockets[SOCKET_TYPE::SOCKET_LEFT_FOOT] = m_pModelCom->Get_HierarchyNode(L"L_Foot_End");
	m_Sockets[SOCKET_TYPE::SOCKET_RIGHT_FIST] = m_pModelCom->Get_HierarchyNode(L"R_Hand_1_Lct");
	m_Sockets[SOCKET_TYPE::SOCKET_RIGHT_FOOT] = m_pModelCom->Get_HierarchyNode(L"R_Foot_End");

	return S_OK;
}

HRESULT CBoss_Akaza::Ready_Parts()
{
	CTrail::TRAIL_DESC TrailDesc = {};
	TrailDesc.bTrail = true;
	TrailDesc.fAccGenTrail = 0.f;
	TrailDesc.fGenTrailTime = 0.01f;
	TrailDesc.vDiffuseColor = { .4f, .25f, .7f, 0.5f };


	// LeftFoot
	m_pTrails[CMonster::SOCKET_TYPE::SOCKET_LEFT_FOOT] = CTrail::Create(m_pDevice, m_pContext, L"LeftFoot_Trail", TrailDesc);
	if (m_pTrails[CMonster::SOCKET_TYPE::SOCKET_LEFT_FOOT] == nullptr)
		return E_FAIL;

	if (FAILED(m_pTrails[CMonster::SOCKET_TYPE::SOCKET_LEFT_FOOT]->Initialize(nullptr)))
		return E_FAIL;

	m_pTrails[CMonster::SOCKET_TYPE::SOCKET_LEFT_FOOT]->SetUp_Position(XMVectorSet(0.f, 0.0f, -0.015f, 1.f), XMVectorSet(0.f, 0.0f, 0.015f, 1.f));
	m_pTrails[CMonster::SOCKET_TYPE::SOCKET_LEFT_FOOT]->Set_VtxCount(66);



	// RightFoot
	m_pTrails[CMonster::SOCKET_TYPE::SOCKET_RIGHT_FOOT] = CTrail::Create(m_pDevice, m_pContext, L"RightFoot_Trail", TrailDesc);
	if (m_pTrails[CMonster::SOCKET_TYPE::SOCKET_RIGHT_FOOT] == nullptr)
		return E_FAIL;

	if (FAILED(m_pTrails[CMonster::SOCKET_TYPE::SOCKET_RIGHT_FOOT]->Initialize(nullptr)))
		return E_FAIL;

	m_pTrails[CMonster::SOCKET_TYPE::SOCKET_RIGHT_FOOT]->SetUp_Position(XMVectorSet(0.f, 0.0f, -0.015f, 1.f), XMVectorSet(0.f, 0.0f, 0.015f, 1.f));
	m_pTrails[CMonster::SOCKET_TYPE::SOCKET_RIGHT_FOOT]->Set_VtxCount(66);

	return S_OK;
}


CBoss_Akaza* CBoss_Akaza::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const MONSTER_STAT& tStat)
{
	CBoss_Akaza* pInstance = new CBoss_Akaza(pDevice, pContext, strObjectTag, tStat);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CBoss_Akaza");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CBoss_Akaza::Clone(void* pArg)
{
	CBoss_Akaza* pInstance = new CBoss_Akaza(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_Akaza");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Akaza::Free()
{
	__super::Free();
}


#include "stdafx.h"
#include "Boss_Enmu.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Sword.h"
#include "Sweath.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"




#include "State_Boss_Idle.h"
#include "State_Monster_Jump.h"
#include "State_Monster_Trace.h"
#include "State_Monster_Regen.h"
#include "State_Monter_Dead.h"

#include "State_Enmu_Attack.h"
#include "State_Enmu_Skill_0.h"
#include "State_Enmu_Skill_1.h"
#include "State_Boss_Battle_Dash.h"

#include "State_Monster_Damaged_Basic.h"
#include "State_Monster_Damaged_AirBorn.h"
#include "State_Monster_Damaged_Blow.h"
#include "State_Monster_Damaged_Bound.h"
#include "State_Monster_Defence_Trace.h"

#include "UI_NextFog.h"

USING(Client)
CBoss_Enmu::CBoss_Enmu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const MONSTER_STAT& tStat)
	: CMonster(pDevice, pContext, strObjectTag, tStat)
{
}

CBoss_Enmu::CBoss_Enmu(const CBoss_Enmu& rhs)
	: CMonster(rhs)
{
}

HRESULT CBoss_Enmu::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CBoss_Enmu::Initialize(void* pArg)
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

	m_eMonsterType = MONSTER_TYPE::ENMU;

	return S_OK;
}

void CBoss_Enmu::Tick(_float fTimeDelta)
{
	

	m_pStateCom->Tick_State(fTimeDelta);

	if (CMonster::IDLE == m_pStateCom->Get_CurrState() && m_iSkillCount > 0)
	{
		_float fRatio = m_tStat.fHp / m_tStat.fMaxHp;
		if (fRatio <= .8f && m_iSkillCount == 3)
		{
			--m_iSkillCount;
			GI->Play_Sound(L"Battle_Finish.wav", CHANNELID::SOUND_UI, 1.f, true);
			GI->Play_Sound(L"Voice_Enmu_Skill_0.wav", CHANNELID::SOUND_VOICE_MONSTER1, 1.f, true);
			m_pStateCom->Change_State(MONSTER_STATE::SKILL_0);
		}
		else if (fRatio <= .5f && m_iSkillCount == 2)
		{
			--m_iSkillCount;
			GI->Play_Sound(L"Battle_Finish.wav", CHANNELID::SOUND_UI, 1.f, true);
			GI->Play_Sound(L"Voice_Enmu_Skill_1.wav", CHANNELID::SOUND_VOICE_MONSTER1, 1.f, true);
			m_pStateCom->Change_State(MONSTER_STATE::SKILL_1);
		}
		else if (fRatio <= .3f && m_iSkillCount == 1)
		{
			--m_iSkillCount;
			GI->Play_Sound(L"Battle_Finish.wav", CHANNELID::SOUND_UI, 1.f, true);
			GI->Play_Sound(L"Voice_Enmu_Skill_0.wav", CHANNELID::SOUND_VOICE_MONSTER1, 1.f, true);
			m_pStateCom->Change_State(MONSTER_STATE::SKILL_0);
		}
	}
	__super::Tick(fTimeDelta);
}

void CBoss_Enmu::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
	m_pRigidBodyCom->LateTick_RigidBody(fTimeDelta);

	m_pRendererCom->Add_Debug(m_pNavigationCom);
}

HRESULT CBoss_Enmu::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	
	return S_OK;
}

void CBoss_Enmu::Enter_Scene()
{
}

void CBoss_Enmu::Return_Pool()
{
	
}

void CBoss_Enmu::Collision_Enter(const COLLISION_INFO& tInfo)
{
	__super::Collision_Enter(tInfo);
}

void CBoss_Enmu::Collision_Continue(const COLLISION_INFO& tInfo)
{
	__super::Collision_Continue(tInfo);
}

void CBoss_Enmu::Collision_Exit(const COLLISION_INFO& tInfo)
{
	__super::Collision_Exit(tInfo);
}

void CBoss_Enmu::On_Damaged(const COLLISION_INFO& tInfo)
{
	__super::On_Damaged(tInfo);
	// GI->Set_Slow(L"Timer_GamePlay", 0.1f, 0.f);
}



HRESULT CBoss_Enmu::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Enmu"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
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

HRESULT CBoss_Enmu::Ready_States()
{
	list<wstring> strAnimationName;

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_BaseNut01_1");
	m_pStateCom->Add_State(CMonster::MONSTER_STATE::IDLE, CState_Boss_Idle::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));


	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_BaseJump01_0");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_BaseJump01_1");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_BaseJump01_2");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_BaseJump01_3");
	
	m_pStateCom->Add_State(CMonster::MONSTER_STATE::JUMP, CState_Monster_Jump::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_AtkAngry01_Cut");
	m_pStateCom->Add_State(CMonster::MONSTER_STATE::REGEN, CState_Monster_Regen::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));

	vector<list<wstring>> AttackAnimations;
	AttackAnimations.reserve(CState_Enmu_Attack::ENMU_ATTACK_TYPE::ATTACK_END);


	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_BaseRun01_1");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_AtkCmbW01");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_AtkCmbW02");
	AttackAnimations.push_back(strAnimationName);

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_AtkSkl05");
	AttackAnimations.push_back(strAnimationName);


	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_AtkSkl03_0");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_AtkSkl03_1");
	AttackAnimations.push_back(strAnimationName);

	m_pStateCom->Add_State(CMonster::ATTACK, CState_Enmu_Attack::Create(m_pDevice, m_pContext, m_pStateCom, AttackAnimations));


	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_AtkSkl04_1");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_BaseNut01_1");
	m_pStateCom->Add_State(CMonster::SKILL_0, CState_Enmu_Skill_0::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_AtkSkl06_0");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_AtkSkl06_1");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_AtkSkl06_2");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_AtkSkl06_3");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_AtkSkl06_4");
	m_pStateCom->Add_State(CMonster::SKILL_1, CState_Enmu_Skill_1::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));


	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_BaseStepL01");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_BaseStepL02");

	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_BaseStepR01");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P1011_V00_C00_BaseStepR02");
	m_pStateCom->Add_State(CMonster::DASH, CState_Boss_Battle_Dash::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));



	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P0000_V00_C00_Dmg02_F");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P0000_V00_C00_Dmg02_F");
	m_pStateCom->Add_State(CMonster::DAMAGED_BASIC, CState_Monster_Damaged_Basic::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));



	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P0000_V00_C00_DmgBlowF01_0");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P0000_V00_C00_DmgBlowF01_1");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P0000_V00_C00_DmgBlowF01_2");
	m_pStateCom->Add_State(CMonster::DAMAGED_AIRBORN, CState_Monster_Damaged_AirBorn::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));



	


	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P0000_V00_C00_DmgStrikeF01_0");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P0000_V00_C00_DmgStrikeF01_1");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P0000_V00_C00_DmgDown01_1");
	m_pStateCom->Add_State(CMonster::DAMAGED_BLOW, CState_Monster_Damaged_Blow::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));

	strAnimationName.clear();
	
   
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P0000_V00_C00_DmgBound01_0");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P0000_V00_C00_DmgBound02_2");
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P0000_V00_C00_DmgDown01_1");
	m_pStateCom->Add_State(CMonster::DAMAGED_BOUND, CState_Monster_Damaged_Bound::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));
	

	strAnimationName.clear();
	strAnimationName.push_back(L"SK_P1011_V00_C00.ao|A_P0000_V00_C00_Death");
	m_pStateCom->Add_State(CMonster::DIE, CState_Monster_Dead::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName));


	m_pStateCom->Change_State(CMonster::MONSTER_STATE::REGEN);

	return S_OK;
}

HRESULT CBoss_Enmu::Ready_Colliders()
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
	ColliderDesc.pNode = m_pModelCom->Get_HierarchyNode(L"R_Hand_1");
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

HRESULT CBoss_Enmu::Ready_Sockets()
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

HRESULT CBoss_Enmu::Ready_Parts()
{
	return S_OK;
}


CBoss_Enmu* CBoss_Enmu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const MONSTER_STAT& tStat)
{
	CBoss_Enmu* pInstance = new CBoss_Enmu(pDevice, pContext, strObjectTag, tStat);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CBoss_Enmu");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CBoss_Enmu::Clone(void* pArg)
{
	CBoss_Enmu* pInstance = new CBoss_Enmu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_Enmu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Enmu::Free()
{
	__super::Free();
}


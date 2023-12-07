#include "stdafx.h"
#include "GameInstance.h"
#include "Monster.h"
#include "HierarchyNode.h"
#include "Part.h"
#include "Effect_Manager.h"
#include "Utils.h"
#include "Camera.h"
#include "Particle_Manager.h"
#include "Camera_Manager.h"
#include "UI_NextFog.h"
#include "UI_Manager.h"
USING(Client)
CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const MONSTER_STAT& tStat)
	: CGameObject(pDevice, pContext, strObjectTag, OBJ_TYPE::OBJ_MONSTER)
{
	m_tStat = tStat;
	m_tStat.fMaxHp = m_tStat.fHp;
	m_tStat.fMaxMp = m_tStat.fMp;
}

CMonster::CMonster(const CMonster& rhs)
	: CGameObject(rhs)
	, m_tStat(rhs.m_tStat)
	, m_fDissolveWeight(1.f)

{	
	
}

HRESULT CMonster::Initialize_Prototype()
{
	if(FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_fDissolveWeight = 1.f;

	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonster::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	GI->Add_CollisionGroup(COLLISION_GROUP::MONSTER, this);

	for (auto& pPart : m_Parts)
		pPart->Tick(fTimeDelta);

	if (m_bInfinite)
	{
		m_fAccInfinite += fTimeDelta;
		if (m_fAccInfinite >= m_fInfiniteTime)
		{
			m_bInfinite = false;
			m_fAccInfinite = 0.f;

			Set_ActiveColliders(CCollider::DETECTION_TYPE::HEAD, true);
			Set_ActiveColliders(CCollider::DETECTION_TYPE::BODY, true);
		}

	}

	if (m_bReserveDead)
	{
		m_fDissolveWeight += 0.2f * fTimeDelta;
		if (m_fDissolveWeight >= 1.f)
		{
			if (m_eMonsterType == CMonster::ENMU || m_eMonsterType == CMonster::AKAZA)
			{
				CUI_NextFog::NEXT_INFO NextInfo;
				NextInfo.eNextLevel = LEVELID::LEVEL_FINAL_BOSS;
				NextInfo.strFolderName = "Final_Boss";
				GI->Stop_All();
				CUI_Manager::GetInstance()->Reset_HpBar(CUI_Manager::GAUGE_BARTYPE::LEFT_HP);
				if (FAILED(GI->Add_GameObject(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_UI, L"Prototype_GameObject_UI_Logo_NextFog", &NextInfo)))
					assert(nullptr);
				
			}

			Set_ActiveColliders(CCollider::DETECTION_TYPE::BOUNDARY, false);
			Set_ActiveColliders(CCollider::DETECTION_TYPE::HEAD, false);
			Set_ActiveColliders(CCollider::DETECTION_TYPE::BODY, false);
			Set_Dead(true);
		}
	}
}

void CMonster::LateTick(_float fTimeDelta)
{
	std::async(&CModel::Play_Animation, m_pModelCom, m_pTransformCom, fTimeDelta);

	for (auto& pPart : m_Parts)
		pPart->LateTick(fTimeDelta);


	for (auto& pPart : m_Parts)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, pPart);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, pPart);
	}
		
	
	__super::LateTick(fTimeDelta);
	if (true == GI->Intersect_Frustum_World(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}

	for (_uint i = 0; i < CCollider::DETECTION_TYPE::DETECTION_END; ++i)
	{
		for (auto& pCollider : m_Colliders[i])
			m_pRendererCom->Add_Debug(pCollider);
	}

}

HRESULT CMonster::Render()
{
	// __super::Render();
	 
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster::Render_ShadowDepth()
{

	if (nullptr == m_pShaderCom || nullptr == m_pTransformCom)
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TransPose(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &GI->Get_ShadowViewMatrix(GI->Get_CurrentLevel()))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GI->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(0), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 10)))
			return E_FAIL;
	}


	return S_OK;
}

void CMonster::Collision_Enter(const COLLISION_INFO& tInfo)
{
	if (tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_CHARACTER || tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_WEAPON)
	{
		if (tInfo.pMyCollider->Get_DetectionType() == CCollider::DETECTION_TYPE::BODY && tInfo.pOtherCollider->Get_DetectionType() == CCollider::DETECTION_TYPE::ATTACK)
		{
			On_Damaged(tInfo);
		}
	}
}

void CMonster::Collision_Continue(const COLLISION_INFO& tInfo)
{

	if (tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_MONSTER || tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_CHARACTER)
	{
		CTransform* pOtherTransform = tInfo.pOther->Get_Component<CTransform>(L"Com_Transform");
		_vector vTargetDir = pOtherTransform->Get_State(CTransform::STATE::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);

		_float fTargetLen = XMVectorGetX(XMVector3Length(vTargetDir));
		if (tInfo.pOtherCollider->Get_DetectionType() == CCollider::BODY
			&& tInfo.pMyCollider->Get_DetectionType() == CCollider::BODY)
		{
			vTargetDir = XMVectorSetY(vTargetDir, 0.f);
			vTargetDir = XMVector3Normalize(vTargetDir);
			vTargetDir *= -1.f;

			_float fForce = tInfo.pMyCollider->Get_Radius() + tInfo.pOtherCollider->Get_Radius() - (fTargetLen);

			if (fForce > 0.f)
			{
				_float fTimeDelta = GI->Get_TimeDelta(L"Timer_GamePlay");
				m_pRigidBodyCom->Set_PushVelocity(vTargetDir * (fForce / 2.f), fTimeDelta);
			}
		}
	}
}

void CMonster::Collision_Exit(const COLLISION_INFO& tInfo)
{

}


CHierarchyNode* CMonster::Get_Socket(const wstring& strSocketName)
{
	for (auto& pSocket : m_Sockets)
	{
		if (pSocket->Get_Name() == strSocketName)
			return pSocket;
	}
	return nullptr;
}

void CMonster::On_Damaged(const COLLISION_INFO& tInfo)
{
	if (true == m_bInfinite)
		return;

	if (MONSTER_STATE::DIE == m_pStateCom->Get_CurrState())
		return;

	m_tStat.fHp -= tInfo.pOtherCollider->Get_Damage();
	if (m_tStat.fHp <= 0.f)
	{
		m_bReserveDead = true;
		m_fDissolveWeight = 0.f;
		m_pStateCom->Change_State(MONSTER_STATE::DIE);

		if (m_eMonsterType == MONSTER_TYPE::ENMU || m_eMonsterType == MONSTER_TYPE::AKAZA)
		{
			GI->Set_Slow(L"Timer_GamePlay", 1.f, 0.1f, true);
			GI->Play_Sound(L"Battle_Finish.wav", CHANNELID::SOUND_UI, 1.f, true);
		}
		return;
	}

	if (tInfo.pOtherCollider->Get_AirBorn_Power() > 0.f)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_Position() + XMVectorSet(0.f, .1f, 0.f, 0.f));
	}

	

	Play_DamagedSound();

	if (tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_WEAPON)
	{
		GI->Play_Sound(L"Slash_Hit.wav", CHANNELID::SOUND_SLASH_HIT, 1.f, true);
		_float fRadianX = XMConvertToRadians(CUtils::Random_Float(-30.f, 30.f));
		_float fRadianY = XMConvertToRadians(CUtils::Random_Float(-30.f, 30.f));
		_float fRadianZ = XMConvertToRadians(CUtils::Random_Float(-30.f, 30.f));

		_vector vQuaternion = XMQuaternionRotationRollPitchYaw(fRadianX, fRadianY, fRadianZ);

		
		_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		WorldMatrix.r[CTransform::STATE_POSITION] += XMVectorSet(0.f, 0.5f, 0.f, 0.f) + XMVector3Normalize(XMVectorSetW(tInfo.pOtherCollider->Get_Position(), 1.f) - WorldMatrix.r[CTransform::STATE_POSITION]) * 0.5f;
		XMVectorSetW(WorldMatrix.r[CTransform::STATE_POSITION], 1.f);

		_int iRandom = CUtils::Random_Int(0, 2);
		CEffect_Manager::GetInstance()->Generate_Effect(L"Basic_Slash_Damaged_" + to_wstring(iRandom), XMMatrixIdentity(), XMMatrixRotationQuaternion(vQuaternion) * WorldMatrix, .2f);
		CParticle_Manager::GetInstance()->Generate_Particle(L"Particle_Sword_Hit_0", WorldMatrix);
	}
	else
	{
		GI->Play_Sound(L"Normal_Hit.wav", CHANNELID::SOUND_NORMAL_HIT, 1.f, true);
		_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		WorldMatrix.r[CTransform::STATE_POSITION] += XMVectorSet(0.f, 0.5f, 0.f, 0.f) + XMVector3Normalize(XMVectorSetW(tInfo.pOtherCollider->Get_Position(), 1.f) - WorldMatrix.r[CTransform::STATE_POSITION]) * 0.5f;
		XMVectorSetW(WorldMatrix.r[CTransform::STATE_POSITION], 1.f);
		CParticle_Manager::GetInstance()->Generate_Particle(L"Particle_Hit_0", WorldMatrix);

		_int iRandomEffect = CUtils::Random_Int(0, 1);
		wstring strHitEffect = L"Basic_Damaged_" + to_wstring(iRandomEffect);

		CEffect_Manager::GetInstance()->Generate_Effect(strHitEffect, XMMatrixIdentity(), WorldMatrix, .5f);
		
	}


	if (true == tInfo.pOtherCollider->Is_HitLag())
	{
		GI->Set_Slow(L"Timer_GamePlay", .3f, .01f, false);
	}


	LookAt_DamagedObject(tInfo.pOther);


	CCamera* pCamera = CCamera_Manager::GetInstance()->Get_MainCamera();
	
	switch (tInfo.pOtherCollider->Get_AttackType())
	{
	case CCollider::ATTACK_TYPE::BASIC:
		m_pRigidBodyCom->Add_Velocity_Acc(
			-1.f * XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f)),
			tInfo.pOtherCollider->Get_PushPower());
		m_pStateCom->Change_State(MONSTER_STATE::DAMAGED_BASIC);
		break;

	case CCollider::ATTACK_TYPE::AIR_BORN:
		m_pRigidBodyCom->Add_Velocity(XMVectorSet(0.f, 1.f, 0.f, 0.f), tInfo.pOtherCollider->Get_AirBorn_Power());
		m_pRigidBodyCom->Add_Velocity_Acc(
			-1.f * XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f)),
			tInfo.pOtherCollider->Get_PushPower());
		m_pStateCom->Change_State(MONSTER_STATE::DAMAGED_AIRBORN);
		if (nullptr != pCamera)
		{
			pCamera->Cam_Shake(1.f, 3.f);
		}
		break;

	case CCollider::ATTACK_TYPE::BLOW:
		m_pRigidBodyCom->Add_Velocity_Acc(
			-1.f * XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f)),
			tInfo.pOtherCollider->Get_PushPower());;

		m_pStateCom->Change_State(MONSTER_STATE::DAMAGED_BLOW);
		break;

	case CCollider::ATTACK_TYPE::BOUND:
		m_pRigidBodyCom->Add_Velocity(XMVectorSet(0.f, 1.f, 0.f, 0.f), tInfo.pOtherCollider->Get_AirBorn_Power());
		m_pRigidBodyCom->Add_Velocity_Acc(
			-1.f * XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f)),
			tInfo.pOtherCollider->Get_PushPower());

		m_pStateCom->Change_State(MONSTER_STATE::DAMAGED_BOUND);
		break;
	}
}




void CMonster::LookAt_DamagedObject(CGameObject* pAttacker)
{
	CTransform* pOtherTransform = pAttacker->Get_Component<CTransform>(L"Com_Transform");
	if (nullptr == pOtherTransform)
		return;

	m_pTransformCom->LookAt_ForLandObject(pOtherTransform->Get_State(CTransform::STATE_POSITION));
}

void CMonster::Play_DamagedSound()
{
	TCHAR strSoundFileName[MAX_PATH] = L"";

	
	

	switch (m_eMonsterType)
	{
	case Client::CMonster::NORMAL_0:
		lstrcatW(strSoundFileName, L"Voice_Normal_Monster_0_Damanged_Basic_");
		lstrcatW(strSoundFileName, to_wstring(CUtils::Random_Int(0, 4)).c_str());
		lstrcatW(strSoundFileName, L".wav");
		GI->Play_Sound(strSoundFileName, CHANNELID::SOUND_VOICE_MONSTER1, 1.f);
		break;
	case Client::CMonster::NORMAL_1:
		lstrcatW(strSoundFileName, L"Voice_Normal_Monster_1_Damanged_Basic_");
		lstrcatW(strSoundFileName, to_wstring(CUtils::Random_Int(0, 5)).c_str());
		lstrcatW(strSoundFileName, L".wav");
		GI->Play_Sound(strSoundFileName, CHANNELID::SOUND_VOICE_MONSTER2, 1.f);
		break;
	case Client::CMonster::NORMAL_2:
		lstrcatW(strSoundFileName, L"Voice_Normal_Monster_2_Damaged");
		lstrcatW(strSoundFileName, L".wav");
		GI->Play_Sound(strSoundFileName, CHANNELID::SOUND_VOICE_MONSTER3, 1.f);
		break;
	case Client::CMonster::ENMU:
		lstrcatW(strSoundFileName, L"Voice_Enmu_Damaged_Basic_");
		lstrcatW(strSoundFileName, to_wstring(CUtils::Random_Int(0, 1)).c_str());
		lstrcatW(strSoundFileName, L".wav");
		GI->Play_Sound(strSoundFileName, CHANNELID::SOUND_VOICE_MONSTER1, 1.f, true);
		break;
	case Client::CMonster::AKAZA:
		lstrcatW(strSoundFileName, L"Voice_Akakza_Damaged_Basic_");
		lstrcatW(strSoundFileName, to_wstring(CUtils::Random_Int(0, 3)).c_str());
		lstrcatW(strSoundFileName, L".wav");
		GI->Play_Sound(strSoundFileName, CHANNELID::SOUND_VOICE_MONSTER1, 1.f, true);
		break;
	default:
		break;
	}

	
}

void CMonster::Free()
{
	__super::Free();

	for (auto& pPart : m_Parts)
		Safe_Release(pPart);



	m_Parts.clear();

}

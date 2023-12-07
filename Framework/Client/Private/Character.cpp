#include "stdafx.h"
#include "GameInstance.h"
#include "Character.h"
#include "HierarchyNode.h"
#include "Key_Manager.h"
#include "Part.h"
#include "Sword.h"
#include "Sweath.h"
#include <future>
#include "Trail.h"
#include "Monster.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"
#include "Camera_Manager.h"
#include "Camera.h"
#include "Utils.h"

USING(Client)
CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, CHARACTER_TYPE eCharacterType)
	: CGameObject(pDevice, pContext, strObjectTag, OBJ_TYPE::OBJ_CHARACTER)
{
	m_eCharacterType = eCharacterType;
}

CCharacter::CCharacter(const CCharacter& rhs)
	: CGameObject(rhs)
	, m_eCharacterType(rhs.m_eCharacterType)
{	

}

HRESULT CCharacter::Initialize_Prototype()
{
	if(FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacter::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_tStat = *((CHARACTER_STAT*)pArg);
	}

	return S_OK;
}

void CCharacter::Tick(_float fTimeDelta)
{
	GI->Add_CollisionGroup(COLLISION_GROUP::CHARACTER, this);

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
	

	for(_uint i = 0; i < SOCKET_END; ++i)
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

void CCharacter::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	if (nullptr == m_pRendererCom)
		return;

	
	std::async(&CModel::Play_Animation, m_pModelCom, m_pTransformCom, fTimeDelta);
	

	for (auto& pPart : m_Parts)
		pPart->LateTick(fTimeDelta);

	for (auto& pPart : m_Parts)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, pPart);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, pPart);
	}
		
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	

	m_pRendererCom->Set_PlayerPosition(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_uint i = 0; i < SOCKET_END; ++i)
	{
		if (nullptr == m_pTrails[i])
			continue;

		m_pTrails[i]->LateTick(fTimeDelta);
	}



	for (_uint i = 0; i < CCollider::DETECTION_TYPE::DETECTION_END; ++i)
	{
		for (auto& pCollider : m_Colliders[i])
			m_pRendererCom->Add_Debug(pCollider);
	}
	m_pRendererCom->Add_Debug(m_pNavigationCom);
}

HRESULT CCharacter::Render()
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
	{
		switch (m_eCharacterType)
		{
		case CHARACTER_TYPE::TANJIRO:
			vRimColor = { 0.f, 0.5f, 1.f, 1.f };
			break;
		case CHARACTER_TYPE::ZENITSU:
			vRimColor = { 0.f, 0.5f, 1.f, 1.f };
			break;
		}
	}
		

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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	

	return S_OK;
}

HRESULT CCharacter::Render_ShadowDepth()
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

void CCharacter::Collision_Enter(const COLLISION_INFO& tInfo)
{
	if (tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_MONSTER || tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_EFFECT)
	{
		if (tInfo.pMyCollider->Get_DetectionType() == CCollider::DETECTION_TYPE::BODY && tInfo.pOtherCollider->Get_DetectionType() == CCollider::DETECTION_TYPE::ATTACK)
		{
			On_Damaged(tInfo);
		}
	}
}

void CCharacter::Collision_Continue(const COLLISION_INFO& tInfo)
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

			if (fForce / 2.f > 0.f)
			{
				_float fTimeDelta = GI->Get_TimeDelta(L"Timer_GamePlay");
				m_pRigidBodyCom->Set_PushVelocity(vTargetDir * (fForce / 2.f), fTimeDelta);
			}
		}
	}


}

void CCharacter::Collision_Exit(const COLLISION_INFO& tInfo)
{
	if (tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_MONSTER
		|| tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_CHARACTER)
	{
		if (tInfo.pOtherCollider->Get_DetectionType() == CCollider::BODY
			&& tInfo.pMyCollider->Get_DetectionType() == CCollider::BODY)
		{
			if (m_pStateCom->Get_CurrState() == CCharacter::STATE::DAMAGED_BLOW)
				return;

			_float3 vVelocity = m_pRigidBodyCom->Get_Velocity();

			vVelocity.x = 0.f;
			vVelocity.z = 0.f;

			m_pRigidBodyCom->Set_Velocity(vVelocity);
		}
	}
}


CHierarchyNode* CCharacter::Get_Socket(PARTTYPE eType)
{
	if (eType >= PARTTYPE::PART_END)
		return nullptr;


	return m_Sockets[eType];
}

CHierarchyNode* CCharacter::Get_Socket(const wstring& strSocketName)
{
	for (auto& pSocket : m_Sockets)
	{
		if (pSocket->Get_Name() == strSocketName)
			return pSocket;
	}
	return nullptr;
}

void CCharacter::DrawSword()
{
	if (nullptr == m_Parts[PARTTYPE::PART_SWORD])
		return;

	CSword* pSword = dynamic_cast<CSword*>(m_Parts[PARTTYPE::PART_SWORD]);
	if (nullptr == pSword)
		return;
	
	if (pSword->Get_Current_SocketBone() == m_Sockets[SOCKET_SWORD])
		return;

	pSword->Set_OriginRotation_Transform(XMMatrixRotationQuaternion(
		XMQuaternionRotationRollPitchYaw(XMConvertToRadians(180.f), XMConvertToRadians(0.f), XMConvertToRadians(-90.f))));

	pSword->Set_SocketBone(m_Sockets[SOCKET_SWORD]);
	pSword->Set_Sweath(false);
}

void CCharacter::SweathSword()
{
	if (nullptr == m_Parts[PARTTYPE::PART_SWORD])
		return;

	CSword* pSword = dynamic_cast<CSword*>(m_Parts[PARTTYPE::PART_SWORD]);
	if (nullptr == pSword)
		return;

	if (pSword->Get_Current_SocketBone() == m_Sockets[SOCKET_SWEATH])
		return;

	pSword->Set_OriginRotation_Transform(XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(pSword->Get_PrevRotation().x, pSword->Get_PrevRotation().y, pSword->Get_PrevRotation().z)));
	pSword->Set_SocketBone(m_Sockets[SOCKET_SWEATH]);

	pSword->Set_Sweath(true);
}



void CCharacter::Generate_Trail(SOCKET_TYPE eSocketType)
{
	if (eSocketType >= SOCKET_TYPE::SOCKET_END)
		return;

	if (nullptr != m_pTrails[eSocketType])
		m_pTrails[eSocketType]->Start_Trail(m_pTransformCom->Get_WorldMatrix());
}

void CCharacter::Stop_Trail(SOCKET_TYPE eSocketType)
{
	if (eSocketType >= SOCKET_TYPE::SOCKET_END)
		return;

	if (nullptr != m_pTrails[eSocketType])
		m_pTrails[eSocketType]->Stop_Trail();

}

void CCharacter::Play_Sound(CCollider::ATTACK_TYPE eAttackType)
{
	
	if (m_eCharacterType == CCharacter::CHARACTER_TYPE::TANJIRO)
	{
		TCHAR strSoundFileName[MAX_PATH] = L"Voice_Tanjiro_Damaged_Basic_";
		lstrcatW(strSoundFileName, to_wstring(CUtils::Random_Int(0, 2)).c_str());
		lstrcatW(strSoundFileName, L".wav");
		GI->Play_Sound(strSoundFileName, CHANNELID::SOUND_VOICE_CHARACTER, 1.f);
	}

	else if (m_eCharacterType == CCharacter::CHARACTER_TYPE::ZENITSU)
	{
		TCHAR strSoundFileName[MAX_PATH] = L"Voice_Zenitsu_Damaged_Basic_";
		lstrcatW(strSoundFileName, to_wstring(CUtils::Random_Int(0, 2)).c_str());
		lstrcatW(strSoundFileName, L".wav");
		GI->Play_Sound(strSoundFileName, CHANNELID::SOUND_VOICE_CHARACTER, 1.f);
	}

	else if (m_eCharacterType == CCharacter::CHARACTER_TYPE::KYOJURO)
	{
		TCHAR strSoundFileName[MAX_PATH] = L"Voice_Kyojuro_Damaged_Basic_";
		lstrcatW(strSoundFileName, to_wstring(CUtils::Random_Int(0, 2)).c_str());
		lstrcatW(strSoundFileName, L".wav");
		GI->Play_Sound(strSoundFileName, CHANNELID::SOUND_VOICE_CHARACTER, 1.f);
	}
}

void CCharacter::Set_Infinite(_float fInfiniteTime, _bool bInfinite)
{
	m_bInfinite = bInfinite;
	m_fInfiniteTime = fInfiniteTime;
	m_fAccInfinite = 0.f;

}


void CCharacter::LookAt_DamagedObject(CGameObject* pAttacker)
{
	CTransform* pOtherTransform = pAttacker->Get_Component<CTransform>(L"Com_Transform");
	if (nullptr == pOtherTransform)
		return;

	m_pTransformCom->LookAt_ForLandObject(pOtherTransform->Get_State(CTransform::STATE_POSITION));
}

void CCharacter::On_Damaged(const COLLISION_INFO& tInfo)
{
	if (m_bInfinite || m_bReserveDead || m_pStateCom->Get_CurrState() == CCharacter::STATE::DIE)
		return;

	m_tStat.fHp -= tInfo.pOtherCollider->Get_Damage();
	if (m_tStat.fHp <= 0.f)
	{
		m_pStateCom->Change_State(STATE::DIE);
		return;
	}

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[CTransform::STATE_POSITION] += XMVectorSet(0.f, 0.5f, 0.f, 0.f) + XMVector3Normalize(XMVectorSetW(tInfo.pOtherCollider->Get_Position(), 1.f) - WorldMatrix.r[CTransform::STATE_POSITION]) * 0.5f;

	_int iRandomEffect = CUtils::Random_Int(0, 1);

	wstring strHitEffect = L"Basic_Damaged_" + to_wstring(iRandomEffect);
	CEffect_Manager::GetInstance()->Generate_Effect(strHitEffect, XMMatrixIdentity(), WorldMatrix, .5f);
	CParticle_Manager::GetInstance()->Generate_Particle(L"Particle_Hit_0", WorldMatrix);
	
	

	if (true == tInfo.pOtherCollider->Is_HitLag())
	{
		if (tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_MONSTER)
		{
			GI->Set_Slow(L"Timer_GamePlay", .3f, .01f, false);
		}
	}

	LookAt_DamagedObject(tInfo.pOther);

	if (tInfo.pOtherCollider->Get_AirBorn_Power() > 0.f)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_Position() + XMVectorSet(0.f, .1f, 0.f, 0.f));
	}

	CCamera* pCamera = CCamera_Manager::GetInstance()->Get_MainCamera();
	if (nullptr != pCamera)
	{
		pCamera->Cam_Shake(1.f, 3.f);
		GI->Play_Sound(L"Shake.wav", CHANNELID::SOUND_SHAKE, 1.f, true);
	}
		
	
	Play_Sound(tInfo.pOtherCollider->Get_AttackType());
	GI->Play_Sound(L"Normal_Hit.wav", CHANNELID::SOUND_NORMAL_HIT, 1.f, true);
	

	switch (tInfo.pOtherCollider->Get_AttackType())
	{
	case CCollider::ATTACK_TYPE::BASIC:
		m_pRigidBodyCom->Add_Velocity(
			-1.f * XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f)),
			tInfo.pOtherCollider->Get_PushPower());
		if(m_pStateCom->Get_CurrState() != CCharacter::STATE::KNOCKDOWN)
			m_pStateCom->Change_State(STATE::DAMAGED_BASIC);
		break;

	case CCollider::ATTACK_TYPE::AIR_BORN:
		m_pRigidBodyCom->Add_Velocity(XMVectorSet(0.f, 1.f, 0.f, 0.f), tInfo.pOtherCollider->Get_AirBorn_Power());
		m_pRigidBodyCom->Add_Velocity_Acc(
			-1.f * XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f)),
			tInfo.pOtherCollider->Get_PushPower());

		if (m_pStateCom->Get_CurrState() != CCharacter::STATE::KNOCKDOWN)
			m_pStateCom->Change_State(STATE::DAMAGED_AIRBORN);

		break;

	case CCollider::ATTACK_TYPE::BLOW:
		m_pRigidBodyCom->Add_Velocity_Acc(
			-1.f * XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f)),

			tInfo.pOtherCollider->Get_PushPower());
		if (m_pStateCom->Get_CurrState() != CCharacter::STATE::KNOCKDOWN)
			m_pStateCom->Change_State(STATE::DAMAGED_BLOW);
		break;

	case CCollider::ATTACK_TYPE::BOUND:
		m_pRigidBodyCom->Add_Velocity(XMVectorSet(0.f, 1.f, 0.f, 0.f), tInfo.pOtherCollider->Get_AirBorn_Power());
		m_pRigidBodyCom->Add_Velocity_Acc(
			-1.f * XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f)),
			tInfo.pOtherCollider->Get_PushPower());
		if (m_pStateCom->Get_CurrState() != CCharacter::STATE::KNOCKDOWN)
			m_pStateCom->Change_State(STATE::DAMAGED_BOUND);
		break;
	}
}


void CCharacter::Free()
{
	__super::Free();

	for (auto& pPart : m_Parts)
		Safe_Release(pPart);

	m_Parts.clear();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

	for (_uint i = 0; i < SOCKET_END; ++i)
	{
		if (nullptr == m_pTrails[i])
			continue;

		Safe_Release(m_pTrails[i]);
	}

}

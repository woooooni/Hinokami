#include "stdafx.h"
#include "GameInstance.h"
#include "Monster.h"
#include "HierarchyNode.h"
#include "Part.h"


USING(Client)
CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const MONSTER_STAT& tStat)
	: CGameObject(pDevice, pContext, strObjectTag, OBJ_TYPE::OBJ_MONSTER)
{
	m_tStat = tStat;
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
			Set_ActiveColliders(CCollider::DETECTION_TYPE::BOUNDARY, false);
			Set_ActiveColliders(CCollider::DETECTION_TYPE::HEAD, false);
			Set_ActiveColliders(CCollider::DETECTION_TYPE::BODY, false);
			Set_Dead(true);
		}
	}
}

void CMonster::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

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
		std::async(&CModel::Play_Animation, m_pModelCom, m_pTransformCom, fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}
}

HRESULT CMonster::Render()
{
	__super::Render();
	 
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
		vRimColor = { 1.f, .2f, .2f, 1.f };

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
				m_pRigidBodyCom->Set_PushVelocity(vTargetDir * fForce, fTimeDelta);
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
	if (m_bInfinite)
		return;

	if (MONSTER_STATE::DIE == m_pStateCom->Get_CurrState())
		return;

	m_tStat.fHp -= tInfo.pOtherCollider->Get_Damage();
	if (m_tStat.fHp <= 0.f)
	{
		m_bReserveDead = true;
		m_fDissolveWeight = 0.f;
		m_pStateCom->Change_State(MONSTER_STATE::DIE);
		return;
	}


	LookAt_DamagedObject(tInfo.pOther);
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

void CMonster::Free()
{
	__super::Free();

	for (auto& pPart : m_Parts)
		Safe_Release(pPart);



	m_Parts.clear();

}

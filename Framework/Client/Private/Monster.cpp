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

{	
	
}

HRESULT CMonster::Initialize_Prototype()
{
	if(FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

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
		m_fDissolveWeight += 0.2f * fTimeDelta;

	if (m_fDissolveWeight >= 1.f)
		Set_Dead(true);

}

void CMonster::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	std::async(&CModel::Play_Animation, m_pModelCom, m_pTransformCom, fTimeDelta);

	for (auto& pPart : m_Parts)
		pPart->LateTick(fTimeDelta);


	for (auto& pPart : m_Parts)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, pPart);

	__super::LateTick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CMonster::Render()
{
	__super::Render();

	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TransPose(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iPassIndex = 0;
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (true == m_bReserveDead)
		{
			iPassIndex = 1;
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


CHierarchyNode* CMonster::Get_Socket(const wstring& strSocketName)
{
	for (auto& pSocket : m_Sockets)
	{
		if (pSocket->Get_Name() == strSocketName)
			return pSocket;
	}
	return nullptr;
}

void CMonster::AirBorne(_float fForce)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition = XMVectorSetY(vPosition, XMVectorGetY(vPosition) + 0.1f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pRigidBodyCom->Add_Velocity_Acc(XMVectorSet(0.f, 1.f, 0.f, 0.f), fForce);
	m_pRigidBodyCom->Set_Ground(false);
	m_pRigidBodyCom->Set_Gravity(true);
}



void CMonster::Free()
{
	__super::Free();

	for (auto& pPart : m_Parts)
		Safe_Release(pPart);

	m_Parts.clear();

}

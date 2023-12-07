#include "stdafx.h"
#include "GameInstance.h"
#include "Akaza_Stone_1.h"


USING(Client)
CAkaza_Stone_1::CAkaza_Stone_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
	: CGameObject(pDevice, pContext, strObjectTag, OBJ_TYPE::OBJ_EFFECT)
{
}

CAkaza_Stone_1::CAkaza_Stone_1(const CAkaza_Stone_1& rhs)
	: CGameObject(rhs)
{

}


HRESULT CAkaza_Stone_1::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAkaza_Stone_1::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Colliders()))
		return E_FAIL;

	return S_OK;
}

void CAkaza_Stone_1::Tick(_float fTimeDelta)
{
	if (false == m_bDeleteStart)
	{
		m_fAccDeleteStart += fTimeDelta;
		if (m_fAccDeleteStart >= m_fDeleteStartTime)
		{
			m_fAccDeleteStart = 0.f;
			m_bDeleteStart = true;
			Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
		}
	}
	
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_Position() + XMVectorSet(0.f, -1.f, 0.f, 0.f) * 1.f * fTimeDelta);
		m_fDissolveWeight += fTimeDelta;
		if (m_fDissolveWeight >= 1.f)
		{
			Set_Dead(true);
			return;
		}

	}
		
}

void CAkaza_Stone_1::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	if (true == GI->Intersect_Frustum_World(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
	{
		m_pRendererCom->Add_RenderGroup_Instancing(CRenderer::RENDER_SHADOW, CRenderer::SHADER_TYPE::MODEL, this, m_pTransformCom->Get_WorldFloat4x4());
		m_pRendererCom->Add_RenderGroup_Instancing(CRenderer::RENDER_NONBLEND, CRenderer::SHADER_TYPE::MODEL, this, m_pTransformCom->Get_WorldFloat4x4());
	}

	GI->Add_CollisionGroup(COLLISION_GROUP::MONSTER, this);

}

HRESULT CAkaza_Stone_1::Render_Instance(CShader* pInstancingShader, CVIBuffer_Instancing* pInstancingBuffer, const vector<_float4x4>& WorldMatrices)
{
	if (nullptr == m_pModelCom || nullptr == pInstancingShader)
		return E_FAIL;

	if (FAILED(pInstancingShader->Bind_RawValue("g_vCamPosition", &GI->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(pInstancingShader->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TransPose(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(pInstancingShader->Bind_RawValue("g_ViewMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(pInstancingShader->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iPassIndex = 0;
		if (FAILED(m_pModelCom->SetUp_OnShader(pInstancingShader, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;


		if (FAILED(m_pModelCom->SetUp_OnShader(pInstancingShader, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			iPassIndex = 0;
		else
			iPassIndex++;

		if (true == m_bDeleteStart)
		{
			iPassIndex = 2;
			_float4 vDissolveColor = _float4(0.6f, 0.f, 0.f, 1.f);
			if (FAILED(pInstancingShader->Bind_RawValue("g_vDissolveColor", &vDissolveColor, sizeof(_float4))))
				return E_FAIL;

			if (FAILED(pInstancingShader->Bind_RawValue("g_fDissolveWeight", &m_fDissolveWeight, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pDissolveTexture->Bind_ShaderResource(pInstancingShader, "g_DissolveTexture")))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render_Instancing(pInstancingShader, i, pInstancingBuffer, WorldMatrices, iPassIndex)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CAkaza_Stone_1::Render_Instance_Shadow(CShader* pInstancingShader, CVIBuffer_Instancing* pInstancingBuffer, const vector<_float4x4>& WorldMatrices)
{
	if (nullptr == m_pModelCom || nullptr == pInstancingShader)
		return E_FAIL;

	_float4 vCamPosition = GI->Get_CamPosition();
	if (FAILED(pInstancingShader->Bind_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(pInstancingShader->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TransPose(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(pInstancingShader->Bind_Matrix("g_ViewMatrix", &GI->Get_ShadowViewMatrix(GI->Get_CurrentLevel()))))
		return E_FAIL;

	if (FAILED(pInstancingShader->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(pInstancingShader, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/

		if (FAILED(m_pModelCom->Render_Instancing(pInstancingShader, i, pInstancingBuffer, WorldMatrices, 10)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CAkaza_Stone_1::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	m_pTransformCom->Set_Sclae(_float3(5.f, 5.f, 5.f));

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Akaza_Skill_Stone_1"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_DissolveTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAkaza_Stone_1::Ready_Colliders()
{
	CCollider_Sphere::SPHERE_COLLIDER_DESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	BoundingSphere tSphere;
	ZeroMemory(&tSphere, sizeof(BoundingSphere));
	tSphere.Radius = 5.f;

	ColliderDesc.pOwnerTransform = m_pTransformCom;

	XMStoreFloat4x4(&ColliderDesc.ModePivotMatrix, XMMatrixIdentity());
	ColliderDesc.tSphere = tSphere;
	ColliderDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::BOUNDARY, &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::ATTACK, &ColliderDesc)))
		return E_FAIL;

	Set_ActiveColliders(CCollider::DETECTION_TYPE::BOUNDARY, true);
	Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);

	Set_Collider_AttackMode(CCollider::ATTACK_TYPE::AIR_BORN, 12.f, 10.f, 1.f);

	return S_OK;
}


CAkaza_Stone_1* CAkaza_Stone_1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
{
	CAkaza_Stone_1* pInstance = new CAkaza_Stone_1(pDevice, pContext, strObjectTag);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CAkaza_Stone_1");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CAkaza_Stone_1::Clone(void* pArg)
{
	CAkaza_Stone_1* pInstance = new CAkaza_Stone_1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNezko");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAkaza_Stone_1::Free()
{
	__super::Free();

}

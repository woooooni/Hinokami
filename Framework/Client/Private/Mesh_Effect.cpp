#include "stdafx.h"
#include "../Public/Mesh_Effect.h"
#include "GameInstance.h"
#include "Mesh.h"


CMesh_Effect::CMesh_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
	: CEffect(pDevice, pContext, strObjectTag, EFFECT_TYPE::EFFECT_MESH)
{
}

CMesh_Effect::CMesh_Effect(const CMesh_Effect& rhs)
	: CEffect(rhs)
	, m_pModelCom(rhs.m_pModelCom)
	, m_strModelFileName(rhs.m_strModelFileName)
	, m_strModelFolderPath(rhs.m_strModelFolderPath)
	, m_strEffectName(rhs.m_strEffectName)
{
	Safe_AddRef(m_pModelCom);
}


HRESULT CMesh_Effect::Initialize_Prototype(const wstring& strEffectName, const wstring& strModelFolderPath, const wstring& strModelFileName, const MESH_EFFECT_DESC& EffectDesc)
{
	m_tMeshEffectDesc = EffectDesc;
	m_strModelFolderPath = strModelFolderPath;
	m_strModelFileName = strModelFileName;
	m_strEffectName = strEffectName;

	if (nullptr == GAME_INSTANCE->Find_Prototype_GameObject(LEVEL_STATIC, TEXT("Prototype_Component_MeshEffect_") + strEffectName))
	{
		if (FAILED(GAME_INSTANCE->Import_Model_Data(LEVEL_STATIC, TEXT("Prototype_Component_MeshEffect_") + strEffectName, CModel::TYPE::TYPE_NONANIM, strModelFolderPath, strModelFileName)))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CMesh_Effect::Initialize(void* pArg)
{
	if (FAILED(Ready_Components(m_strModelFolderPath, m_strModelFileName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh_Effect::Ready_Components(const wstring& strModelFolderPath, const wstring& strModelFileName)
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Mesh_Effect"), TEXT("Com_EffectShader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MeshEffect_") + m_strEffectName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}


void CMesh_Effect::Tick(_float fTimeDelta)
{
	Update_MeshUV(fTimeDelta);
}

void CMesh_Effect::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CMesh_Effect::Render()
{
	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_tMeshEffectDesc", &m_tMeshEffectDesc, sizeof(MESH_EFFECT_DESC))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (nullptr != m_pModelCom->Get_MaterialTexture(i, aiTextureType_OPACITY))
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_OPACITY, "g_AlphaTexture")))
				return E_FAIL;
		}
		

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}
	

	return S_OK;
}


CMesh_Effect* CMesh_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
	const wstring& strEffectName, 
	const wstring& strObjectTag, 
	const wstring& strFolderPath, 
	const wstring& strFileName, 
	const MESH_EFFECT_DESC& EffectDesc)
{
	CMesh_Effect* pInstance = new CMesh_Effect(pDevice, pContext, strObjectTag);
	if (FAILED(pInstance->Initialize_Prototype(strEffectName, strFolderPath, strFileName, EffectDesc)))
	{
		MSG_BOX("Create Failed : CMesh_Effect");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CMesh_Effect::Clone(void* pArg)
{
	CMesh_Effect* pInstance = new CMesh_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh_Effect::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
}

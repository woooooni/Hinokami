#include "stdafx.h"
#include "GameInstance.h"
#include "Prop.h"
#include "HierarchyNode.h"
#include "Key_Manager.h"


USING(Client)
CProp::CProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
	: CGameObject(pDevice, pContext, strObjectTag)
{
}

CProp::CProp(const CProp& rhs)
	: CGameObject(rhs)
	, m_pShaderCom(rhs.m_pShaderCom)
	, m_pRendererCom(rhs.m_pRendererCom)
	, m_pTransformCom(rhs.m_pTransformCom)
	, m_pModelCom(rhs.m_pModelCom)
{	
	Safe_AddRef(m_pShaderCom);
	Safe_AddRef(m_pRendererCom);
	Safe_AddRef(m_pTransformCom);
	Safe_AddRef(m_pModelCom);
}

HRESULT CProp::Initialize_Prototype()
{
	if(FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProp::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CProp::Tick(_float fTimeDelta)
{

}

void CProp::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CProp::Render()
{
	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/

		if (FAILED(m_pModelCom->SetUpAnimation_OnShader(m_pShaderCom)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CProp::Ready_Components()
{
	return S_OK;
}


CProp* CProp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
{
	CProp* pInstance = new CProp(pDevice, pContext, strObjectTag);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CProp");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CProp::Clone(void* pArg)
{
	CProp* pInstance = new CProp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNezko");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProp::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

}

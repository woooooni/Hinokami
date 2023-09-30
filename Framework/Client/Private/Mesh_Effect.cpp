#include "stdafx.h"
#include "../Public/Mesh_Effect.h"
#include "GameInstance.h"
#include "Mesh.h"


CMesh_Effect::CMesh_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
	: CEffect(pDevice, pContext, strObjectTag)
{
}

CMesh_Effect::CMesh_Effect(const CMesh_Effect& rhs)
	: CEffect(rhs)
	, m_pMesh(rhs.m_pMesh)
{
	Safe_AddRef(m_pMesh);
}


HRESULT CMesh_Effect::Initialize_Prototype(const EFFECT_DESC& EffectDesc)
{
	m_tEffectDesc.vAccUV = EffectDesc.vAccUV;
	m_tEffectDesc.fUVSpeed = EffectDesc.fUVSpeed;
	return S_OK;
}


HRESULT CMesh_Effect::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh_Effect::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"), TEXT("Com_EffectShader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}


void CMesh_Effect::Tick(_float fTimeDelta)
{

}

void CMesh_Effect::LateTick(_float fTimeDelta)
{
	/*if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);*/


}

HRESULT CMesh_Effect::Render()
{
	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	m_pMesh->Render();

	return S_OK;
}


CMesh_Effect* CMesh_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const EFFECT_DESC& EffectDesc)
{
	CMesh_Effect* pInstance = new CMesh_Effect(pDevice, pContext, strObjectTag);
	if (FAILED(pInstance->Initialize_Prototype(EffectDesc)))
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
	Safe_Release(m_pMesh);
}

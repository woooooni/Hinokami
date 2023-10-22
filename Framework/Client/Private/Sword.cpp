#include "stdafx.h"
#include "..\Public\Sword.h"

#include "GameInstance.h"
#include "HierarchyNode.h"

CSword::CSword(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strObjectTag)
	: CPart(pDevice, pContext, strObjectTag, OBJ_TYPE::OBJ_WEAPON)
{

}

CSword::CSword(const CSword & rhs)
	: CPart(rhs)
	, m_strModelPrototype(rhs.m_strModelPrototype)
{

}

HRESULT CSword::Initialize_Prototype(const wstring& strPrototypeSwordModel)
{
	m_strModelPrototype = strPrototypeSwordModel;
	return S_OK;
}

HRESULT CSword::Initialize(void* pArg)
{
	SWORD_DESC* pWeaponDesc = nullptr;
	if (nullptr != pArg)
	{
		pWeaponDesc = (SWORD_DESC*)pArg;

		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pWeaponDesc != nullptr)
	{
		m_pTransformCom->Set_Rotation(XMLoadFloat3(&pWeaponDesc->vRotationDegree));

		m_vPrevRotation = pWeaponDesc->vRotationDegree;
	}
	else
		return E_FAIL;

	return S_OK;
}

void CSword::Tick(_float fTimeDelta)
{
	_matrix		WorldMatrix = m_pSocketBone->Get_CombinedTransformation() * XMLoadFloat4x4(&m_SocketPivotMatrix);

	WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);
	
	Compute_RenderMatrix(m_pTransformCom->Get_WorldMatrix() * WorldMatrix);
}

void CSword::LateTick(_float fTimeDelta)
{

	
}

HRESULT CSword::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}

	return S_OK;
}


void CSword::Generate_Trail()
{
}

void CSword::Stop_Trail()
{
}

void CSword::Generate_Effect()
{
}

HRESULT CSword::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strModelPrototype,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSword::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	return S_OK;
}

CSword * CSword::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strObjectTag, const wstring& strPrototypeSwordModel)
{
	CSword*	pInstance = new CSword(pDevice, pContext, strObjectTag);

	if (FAILED(pInstance->Initialize_Prototype(strPrototypeSwordModel)))
	{
		MSG_BOX("Failed to Created : CSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSword::Clone(void* pArg)
{
	CSword*	pInstance = new CSword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSword::Free()
{
	__super::Free();
}

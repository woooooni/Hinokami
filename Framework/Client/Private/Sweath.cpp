#include "stdafx.h"
#include "..\Public\Sweath.h"

#include "GameInstance.h"
#include "HierarchyNode.h"

CSweath::CSweath(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strObjectTag)
	: CPart(pDevice, pContext, strObjectTag, OBJ_TYPE::OBJ_WEAPON)
{

}

CSweath::CSweath(const CSweath & rhs)
	: CPart(rhs)
	, m_strModelPrototype(rhs.m_strModelPrototype)
{

}

HRESULT CSweath::Initialize_Prototype(const wstring& strPrototypeModelName)
{
	m_strModelPrototype = strPrototypeModelName;
	return S_OK;
}

HRESULT CSweath::Initialize(void* pArg)
{
	SWEATH_DESC* pWeaponDesc = nullptr;
	if (nullptr != pArg)
	{
		pWeaponDesc = (SWEATH_DESC*)pArg;

		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;


	if (pWeaponDesc != nullptr)
	{
		m_pTransformCom->Set_Rotation(XMLoadFloat3(&pWeaponDesc->vRotationDegree));

		m_vPrevRotation = pWeaponDesc->vRotationDegree;
		m_OriginRotationTransform = m_pTransformCom->Get_WorldFloat4x4();
	}
	else
		return E_FAIL;

	return S_OK;
	

	return S_OK;
}

void CSweath::Tick(_float fTimeDelta)
{
	_matrix		WorldMatrix = m_pSocketBone->Get_CombinedTransformation() * XMLoadFloat4x4(&m_SocketPivotMatrix);

	WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);
	
	Compute_RenderMatrix(WorldMatrix);
}

void CSweath::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CSweath::Render()
{
	__super::Render();

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

HRESULT CSweath::Ready_Components()
{

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strModelPrototype,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

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

	return S_OK;
}

HRESULT CSweath::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TransPose(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;


	return S_OK;
}

CSweath * CSweath::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strObjectTag, const wstring& strPrototypeModelName)
{
	CSweath*	pInstance = new CSweath(pDevice, pContext, strObjectTag);

	if (FAILED(pInstance->Initialize_Prototype(strPrototypeModelName)))
	{
		MSG_BOX("Failed to Created : CSweath");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSweath::Clone(void* pArg)
{
	CSweath*	pInstance = new CSweath(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSweath");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSweath::Free()
{
	__super::Free();
}

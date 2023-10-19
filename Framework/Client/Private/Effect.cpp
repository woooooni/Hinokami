#include "stdafx.h"
#include "Effect.h"
#include "GameInstance.h"



CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, 
	EFFECT_TYPE eType, const wstring& strPrototypeModelName)
	: CGameObject(pDevice, pContext, strObjectTag, OBJ_TYPE::OBJ_EFFECT)
	, m_eType(eType)
	, m_strModelPrototype(strPrototypeModelName)
{

}

CEffect::CEffect(const CEffect& rhs)
	: CGameObject(rhs)
	, m_eType(rhs.m_eType)
	, m_tEffectDesc(rhs.m_tEffectDesc)
	, m_strModelPrototype(rhs.m_strModelPrototype)
	, m_iPassIndex(rhs.m_iPassIndex)
{

}

HRESULT CEffect::Initialize_Prototype(const EFFECT_DESC& tEffectDesc)
{
	m_tEffectDesc = tEffectDesc;
	return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CEffect::Tick(_float fTimeDelta)
{
	if (m_bIncrement)
		Increment(fTimeDelta);
	else
		Decrement(fTimeDelta);

	
	_vector vMoveDir = XMVector3Normalize(XMLoadFloat3(&m_tEffectDesc.vMoveDir));
	_vector vTurnDir = XMVector3Normalize(XMLoadFloat3(&m_tEffectDesc.vTurnDir));

	m_pTransformCom->Go_Dir(vMoveDir, m_tEffectDesc.fMoveSpeed, fTimeDelta);
	m_pTransformCom->Turn(vTurnDir, m_tEffectDesc.fTurnSpeed, fTimeDelta);
}

void CEffect::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffect::Render()
{
	if (FAILED(Bind_ShaderResource()))
		return E_FAIL;

	if (m_eType == EFFECT_TYPE::EFFECT_MESH)
	{
		_uint iNumMesh = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMesh; ++i)
			m_pModelCom->Render(m_pShaderCom, i, m_iPassIndex);
	}
	else
	{
		m_pShaderCom->Begin(m_iPassIndex);
		m_pVIBufferCom->Render();
	}

	return S_OK;
}

HRESULT CEffect::Save_EffectInfo()
{
	return S_OK;
}

void CEffect::Increment(_float fTimeDelta)
{
	m_tEffectDesc.fAccIndex += m_tEffectDesc.fIndexSpeed * fTimeDelta;

	if (m_tEffectDesc.fAccIndex >= 1.f)
	{
		m_tEffectDesc.fAccIndex = 0.f;
		m_tEffectDesc.fUVIndex.x++;
		if (m_tEffectDesc.fMaxCountX <= m_tEffectDesc.fUVIndex.x)
		{
			m_tEffectDesc.fUVIndex.x = 0;
			m_tEffectDesc.fUVIndex.y++;
			if (m_tEffectDesc.fMaxCountY <= m_tEffectDesc.fUVIndex.y)
			{
				m_tEffectDesc.fUVIndex.y = 0;
				m_bEnd = true;
			}
		}
	}
}

void CEffect::Decrement(_float fTimeDelta)
{
	m_tEffectDesc.fAccIndex += m_tEffectDesc.fIndexSpeed * fTimeDelta;

	if (m_tEffectDesc.fAccIndex >= 1.f)
	{
		m_tEffectDesc.fAccIndex = 0.f;
		m_tEffectDesc.fUVIndex.x--;
		if (0 > m_tEffectDesc.fUVIndex.x)
		{
			m_tEffectDesc.fUVIndex.x = m_tEffectDesc.fMaxCountX;
			m_tEffectDesc.fUVIndex.y--;
			if (0 < m_tEffectDesc.fUVIndex.y)
			{
				m_tEffectDesc.fUVIndex.y = m_tEffectDesc.fMaxCountY;
				m_bEnd = true;
			}
		}
	}
}

HRESULT CEffect::Bind_ShaderResource()
{
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaxCountX", &m_tEffectDesc.fMaxCountX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaxCountY", &m_tEffectDesc.fMaxCountY, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_tEffectDesc.fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVIndex", &m_tEffectDesc.fUVIndex, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAdditiveDiffuseColor", &m_tEffectDesc.fAdditiveDiffuseColor, sizeof(_float3))))
		return E_FAIL;


	{

		if (-1 < m_tEffectDesc.iDiffuseTextureIndex)
		{
			if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tEffectDesc.iDiffuseTextureIndex)))
				return E_FAIL;
		}


		if (-1 < m_tEffectDesc.iAlphaTextureIndex)
		{
			if (FAILED(m_pAlphaTextureCom->Bind_ShaderResource(m_pShaderCom, "g_AlphaTexture", m_tEffectDesc.iAlphaTextureIndex)))
				return E_FAIL;
		}


		// 둘다 없다면.
		if (-1 == m_tEffectDesc.iDiffuseTextureIndex && -1 == m_tEffectDesc.iAlphaTextureIndex)
			m_iPassIndex = 0;

		// 디퓨즈 텍스쳐만 있다면.
		if (-1 != m_tEffectDesc.iDiffuseTextureIndex && -1 == m_tEffectDesc.iAlphaTextureIndex)
			m_iPassIndex = 1;

		// 알파 텍스쳐만 있다면.
		if (-1 == m_tEffectDesc.iDiffuseTextureIndex && -1 != m_tEffectDesc.iAlphaTextureIndex)
			m_iPassIndex = 2;

		if (-1 != m_tEffectDesc.iDiffuseTextureIndex && -1 != m_tEffectDesc.iAlphaTextureIndex)		
			m_iPassIndex = 3;
	}

	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMLoadFloat4x4(&m_tEffectDesc.OffsetMatrix) * m_pTransformCom->Get_WorldMatrix());
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	return S_OK;
}



HRESULT CEffect::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"), TEXT("Com_Diffuse_Texture"), (CComponent**)&m_pDiffuseTextureCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"), TEXT("Com_Alpha_Texture"), (CComponent**)&m_pAlphaTextureCom)))
		return E_FAIL;



	if(m_eType == EFFECT_TYPE::EFFECT_MESH)
	{
		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Mesh_Effect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;


		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strModelPrototype, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	else
	{
		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Texture_Effect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;


		/* For.Com_VIBuffer */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
			return E_FAIL;
	}
	

	return S_OK;
}


CEffect* CEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, 
	EFFECT_TYPE eType, const wstring& strPrototypeModelName, const EFFECT_DESC& tEffectDesc)
{
	CEffect* pInstance = new CEffect(pDevice, pContext, strObjectTag, eType, strPrototypeModelName);
	if (FAILED(pInstance->Initialize_Prototype(tEffectDesc)))
	{
		MSG_BOX("Failed to Created : CEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect::Clone(void* pArg)
{
	CEffect* pInstance = new CEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pAlphaTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pModelCom);
}






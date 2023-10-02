#include "stdafx.h"
#include "Effect.h"
#include "GameInstance.h"
#include "Mesh.h"


CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, EFFECT_TYPE eType)
	: CGameObject(pDevice, pContext, strObjectTag)
	, m_eType(eType)
{
}

CEffect::CEffect(const CEffect& rhs)
	: CGameObject(rhs)
	, m_pShaderCom(rhs.m_pShaderCom)
	, m_pRendererCom(rhs.m_pRendererCom)
	, m_pTransformCom(rhs.m_pTransformCom)
	, m_pEffectTexture(rhs.m_pEffectTexture)
	, m_eType(rhs.m_eType)
{
	Safe_AddRef(m_pShaderCom);
	Safe_AddRef(m_pRendererCom);
	Safe_AddRef(m_pTransformCom);
	Safe_AddRef(m_pEffectTexture);
}

HRESULT CEffect::Initialize_Prototype()
{
	return S_OK;
}

void CEffect::Tick(_float fTimeDelta)
{
	

}

void CEffect::IncrementUV(_float fTimeDelta)
{
	m_tTextureEffectDesc.fAccUVIndex += fTimeDelta;

	if (m_tTextureEffectDesc.fAccUVIndex >= m_tTextureEffectDesc.fNextIndexSpeed)
	{
		m_tTextureEffectDesc.fAccUVIndex = 0.f;
		m_tTextureEffectDesc.fUVIndex.x += 1.f;
		if (m_tTextureEffectDesc.fUVIndex.x >= m_tTextureEffectDesc.iMaxCountX)
		{
			m_tTextureEffectDesc.fUVIndex.x = 0.f;
			m_tTextureEffectDesc.fUVIndex.y += 1.f;
			if (m_tTextureEffectDesc.fUVIndex.y >= m_tTextureEffectDesc.iMaxCountY)
			{
				m_tTextureEffectDesc.fUVIndex.y = 0.f;
				m_bEnd = true;
			}
		}
	}
}

void CEffect::DecrementUV(_float fTimeDelta)
{
	m_tTextureEffectDesc.fAccUVIndex += fTimeDelta;
	if (m_tTextureEffectDesc.fAccUVIndex >= m_tTextureEffectDesc.fNextIndexSpeed)
	{
		m_tTextureEffectDesc.fAccUVIndex = 0.f;
		m_tTextureEffectDesc.fUVIndex.x -= 1.f;
		if (m_tTextureEffectDesc.fUVIndex.x < 0.f)
		{
			m_tTextureEffectDesc.fUVIndex.x = m_tTextureEffectDesc.iMaxCountX;
			m_tTextureEffectDesc.fUVIndex.y -= 1.f;
			if (m_tTextureEffectDesc.fUVIndex.y < 0.f)
			{
				m_tTextureEffectDesc.fUVIndex.y = 0.f;
				m_bEnd = true;
			}
		}
	}
}

void CEffect::Update_MeshUV(_float fTimeDelta)
{
	m_tMeshEffectDesc.vUVAcc.x += m_tMeshEffectDesc.vUVSpeed.x * fTimeDelta;
	m_tMeshEffectDesc.vUVAcc.y += m_tMeshEffectDesc.vUVSpeed.y * fTimeDelta;
}


void CEffect::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pEffectTexture);
}

HRESULT CEffect::Ready_Components()
{
	return S_OK;
}




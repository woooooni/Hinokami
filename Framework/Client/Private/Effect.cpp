#include "stdafx.h"
#include "Effect.h"
#include "GameInstance.h"
#include "Mesh.h"


CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, OBJ_TYPE eObjType, EFFECT_TYPE eType)
	: CGameObject(pDevice, pContext, strObjectTag, eObjType)
	, m_eType(eType)
{
}

CEffect::CEffect(const CEffect& rhs)
	: CGameObject(rhs)
	, m_pShaderCom(rhs.m_pShaderCom)
	, m_pRendererCom(rhs.m_pRendererCom)
	, m_pTransformCom(rhs.m_pTransformCom)
	, m_eType(rhs.m_eType)
	, m_tMeshEffectDesc(rhs.m_tMeshEffectDesc)
	, m_tTextureEffectDesc(rhs.m_tTextureEffectDesc)
{
	Safe_AddRef(m_pShaderCom);
	Safe_AddRef(m_pRendererCom);
	Safe_AddRef(m_pTransformCom);
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
		if (m_tTextureEffectDesc.fUVIndex.x >= m_tTextureEffectDesc.fMaxCountX)
		{
			m_tTextureEffectDesc.fUVIndex.x = 0.f;
			m_tTextureEffectDesc.fUVIndex.y += 1.f;
			if (m_tTextureEffectDesc.fUVIndex.y >= m_tTextureEffectDesc.fMaxCountY)
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
			m_tTextureEffectDesc.fUVIndex.x = m_tTextureEffectDesc.fMaxCountX;
			m_tTextureEffectDesc.fUVIndex.y -= 1.f;
			if (m_tTextureEffectDesc.fUVIndex.y < 0.f)
			{
				m_tTextureEffectDesc.fUVIndex.y = m_tTextureEffectDesc.fMaxCountY;
				m_bEnd = true;
			}
		}
	}
}

void CEffect::Update_MeshUV(_float fTimeDelta)
{
	m_tMeshEffectDesc.vAccUV.x += m_tMeshEffectDesc.vSpeedUV.x * fTimeDelta;
	m_tMeshEffectDesc.vAccUV.y += m_tMeshEffectDesc.vSpeedUV.y * fTimeDelta;
}


void CEffect::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}

HRESULT CEffect::Ready_Components()
{
	return S_OK;
}




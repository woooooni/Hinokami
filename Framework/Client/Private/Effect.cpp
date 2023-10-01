#include "stdafx.h"
#include "Effect.h"
#include "GameInstance.h"
#include "Mesh.h"


CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
	: CGameObject(pDevice, pContext, strObjectTag)
{
}

CEffect::CEffect(const CEffect& rhs)
	: CGameObject(rhs)
	, m_pShaderCom(rhs.m_pShaderCom)
	, m_pRendererCom(rhs.m_pRendererCom)
	, m_pTransformCom(rhs.m_pTransformCom)
	, m_pEffectTexture(rhs.m_pEffectTexture)
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




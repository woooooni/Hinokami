#include "stdafx.h"
#include "..\Public\Rect_Effect.h"
#include "GameInstance.h"
#include "Mesh.h"



CRect_Effect::CRect_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
	: CEffect(pDevice, pContext, strObjectTag)
{
}

CRect_Effect::CRect_Effect(const CRect_Effect& rhs)
	: CEffect(rhs)
	, m_pRectBuffer(rhs.m_pRectBuffer)
{
	Safe_AddRef(m_pRectBuffer);
}

HRESULT CRect_Effect::Initialize_Prototype(const EFFECT_DESC& EffectDesc)
{

	m_tEffectDesc.vAccUV = EffectDesc.vAccUV;
	m_tEffectDesc.fUVSpeed = EffectDesc.fUVSpeed;

	return S_OK;
}

HRESULT CRect_Effect::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRect_Effect::Ready_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_EffectShader"), (CComponent**)&m_pRectBuffer)))
		return E_FAIL;

	return S_OK;
}


void CRect_Effect::Tick(_float fTimeDelta)
{
	
}

void CRect_Effect::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	
}

HRESULT CRect_Effect::Render()
{
	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	m_pRectBuffer->Render();

	return S_OK;
}


CRect_Effect* CRect_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const EFFECT_DESC& EffectDesc)
{
	CRect_Effect* pInstance = new CRect_Effect(pDevice, pContext, strObjectTag);
	if (FAILED(pInstance->Initialize_Prototype(EffectDesc)))
	{
		MSG_BOX("Create Failed : CRect_Effect");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CRect_Effect::Clone(void* pArg)
{
	CRect_Effect* pInstance = new CRect_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRect_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRect_Effect::Free()
{
	__super::Free();
	Safe_Release(m_pRectBuffer);
}

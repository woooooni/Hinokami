#include "stdafx.h"
#include "UI_Loading_Background.h"
#include "GameInstance.h"
#include "Utils.h"

CUI_Loading_BackGround::CUI_Loading_BackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext, L"UI_Loading_BackGround")
{

}

CUI_Loading_BackGround::CUI_Loading_BackGround(const CUI_Loading_BackGround & rhs)
	: CUI(rhs)
{
	
}

HRESULT CUI_Loading_BackGround::Initialize_Prototype(const UI_INFO& tInfo)
{
	m_tInfo = tInfo;

	
	return S_OK;
}

HRESULT CUI_Loading_BackGround::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_pTransformCom->Set_Scale(XMLoadFloat3(&_float3(m_tInfo.fCX, m_tInfo.fCY, 1.f)));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_tInfo.fX - g_iWinSizeX * 0.5f, -m_tInfo.fY + g_iWinSizeY * 0.5f, 1.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	m_iTextureIndex = CUtils::Random_Int(0, m_pTextureCom->Get_TextureCount() - 1);

	return S_OK;
}

void CUI_Loading_BackGround::Tick(_float fTimeDelta)
{
	// Debug_Input(fTimeDelta);
}

void CUI_Loading_BackGround::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RENDER_UI, this);
}

HRESULT CUI_Loading_BackGround::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex)))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Loading_BackGround::Ready_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_BackGround"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loading_BackGround::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

CUI_Loading_BackGround * CUI_Loading_BackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const UI_INFO& tInfo)
{
	CUI_Loading_BackGround*	pInstance = new CUI_Loading_BackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(tInfo)))
	{
		MSG_BOX("Failed to Created : CUI_Loading_BackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Loading_BackGround::Clone(void* pArg)
{
	CUI_Loading_BackGround*	pInstance = new CUI_Loading_BackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Loading_BackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Loading_BackGround::Free()
{
	__super::Free();
}

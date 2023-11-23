#include "stdafx.h"
#include "UI_Cursor.h"
#include "GameInstance.h"

CUI_Cursor::CUI_Cursor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext, L"UI_Logo_BackGround")
{

}

CUI_Cursor::CUI_Cursor(const CUI_Cursor & rhs)
	: CUI(rhs)
{
	
}

HRESULT CUI_Cursor::Initialize_Prototype(const UI_INFO& tInfo)
{
	m_tInfo = tInfo;

	return S_OK;
}

HRESULT CUI_Cursor::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_pTransformCom->Set_Scale(XMLoadFloat3(&_float3(m_tInfo.fCX, m_tInfo.fCY, 1.f)));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_tInfo.fX - g_iWinSizeX * 0.5f, -m_tInfo.fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	ShowCursor(false);

	return S_OK;
}

void CUI_Cursor::Tick(_float fTimeDelta)
{
	
		
	if (m_bShow)
	{
		m_fAccHide += fTimeDelta;
		if (m_fAccHide >= m_fHideTime)
		{
			m_bShow = false;
			m_fAccHide = 0.f;
		}
	}

	_long	MouseMove = 0;
	if (MouseMove = GI->Get_DIMMoveState(DIMM_X))
	{
		m_fAccHide = 0.f;
		m_bShow = true;
	}

	if (MouseMove = GI->Get_DIMMoveState(DIMM_Y))
	{

		m_fAccHide = 0.f;
		m_bShow = true;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(GI->GetMousePos().x - g_iWinSizeX * 0.5f, -GI->GetMousePos().y + g_iWinSizeY * 0.5f, 0.998f, 1.f));
	
}

void CUI_Cursor::LateTick(_float fTimeDelta)
{
	if (GI->Get_CurrentLevel() == LEVEL_TOOL)
	{
		ShowCursor(true);
	}
	else
	{
		if (GI->Get_CurrentLevel() == LEVEL_FINAL_BOSS || GI->Get_CurrentLevel() == LEVEL_TRAIN || GI->Get_CurrentLevel() == LEVEL_TRAIN_BOSS)
			return;

		if (true == m_bShow)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RENDER_UI, this);
		}
	}
	
}

HRESULT CUI_Cursor::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Cursor::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cursor"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Cursor::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	return S_OK;
}

CUI_Cursor * CUI_Cursor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const UI_INFO& tInfo)
{
	CUI_Cursor*	pInstance = new CUI_Cursor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(tInfo)))
	{
		MSG_BOX("Failed to Created : CUI_Cursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Cursor::Clone(void* pArg)
{
	CUI_Cursor*	pInstance = new CUI_Cursor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Cursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Cursor::Free()
{
	__super::Free();
}
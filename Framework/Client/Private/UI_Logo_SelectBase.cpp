#include "stdafx.h"
#include "UI_Logo_SelectBase.h"
#include "GameInstance.h"
#include "UI_NextFog.h"

CUI_Logo_SelectBase::CUI_Logo_SelectBase(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext, L"UI_Logo_Title")
{

}

CUI_Logo_SelectBase::CUI_Logo_SelectBase(const CUI_Logo_SelectBase & rhs)
	: CUI(rhs)
{
	
}

HRESULT CUI_Logo_SelectBase::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Logo_SelectBase::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		UI_INFO* pUIInfo = (UI_INFO*)pArg;
		memcpy(&m_tInfo, pUIInfo, sizeof(UI_INFO));
	}
	
	m_pTransformCom->Set_Scale(XMLoadFloat3(&_float3(m_tInfo.fCX, m_tInfo.fCY, 1.f)));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_tInfo.fX - g_iWinSizeX * 0.5f, -m_tInfo.fY + g_iWinSizeY * 0.5f, 0.1f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));


	return S_OK;
}

void CUI_Logo_SelectBase::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	//Debug_Input(fTimeDelta);
}

void CUI_Logo_SelectBase::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RENDER_UI, this);


}

HRESULT CUI_Logo_SelectBase::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	if (m_eMouseState == CUI::MOUSE_END)
	{
		_float2 vFontPosition = _float2(m_tInfo.fX - 50.f, m_tInfo.fY - 10.f);
		GI->Render_Fonts(L"Basic", m_strText, vFontPosition, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}
	else
	{
		_float2 vFontPosition = _float2(m_tInfo.fX - 50.f, m_tInfo.fY - 10.f);
		GI->Render_Fonts(L"Basic", m_strText, vFontPosition, XMVectorSet(1.f, 1.f, 1.f, 1.f));
	}
	

	

	return S_OK;
}

void CUI_Logo_SelectBase::On_MouseEnter(_float fTimeDelta)
{

}

void CUI_Logo_SelectBase::On_Mouse(_float fTimeDelta)
{
	if (KEY_TAP(KEY::LBTN))
	{
		if (m_strText == L"게임시작")
		{
			CUI_NextFog::NEXT_INFO NextInfo;
			NextInfo.eNextLevel = LEVELID::LEVEL_TRAIN_STATION;
			NextInfo.strFolderName = "Train_Station";
			GI->Add_GameObject(LEVEL_LOGO, LAYER_TYPE::LAYER_UI, L"Prototype_GameObject_UI_Logo_NextFog", &NextInfo);
		}
		else if (m_strText == L"툴")
		{
			CUI_NextFog::NEXT_INFO NextInfo;
			NextInfo.eNextLevel = LEVELID::LEVEL_TOOL;
			NextInfo.strFolderName = "";

			GI->Add_GameObject(LEVEL_LOGO, LAYER_TYPE::LAYER_UI, L"Prototype_GameObject_UI_Logo_NextFog", &NextInfo);
		}
		else if (m_strText == L"설정")
		{
			// GI->Add_GameObject(LEVEL_LOGO, LAYER_TYPE::LAYER_UI, L"Prototype_GameObject_UI_Logo_NextFog", nullptr);
		}
		else if (m_strText == L"나가기")
		{
			// GI->Add_GameObject(LEVEL_LOGO, LAYER_TYPE::LAYER_UI, L"Prototype_GameObject_UI_Logo_NextFog", nullptr);
		}
	}
}

void CUI_Logo_SelectBase::On_MouseExit(_float fTimeDelta)
{

}

HRESULT CUI_Logo_SelectBase::Ready_Components()
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

	/* Com_NonSelectedTexture*/
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_NonSelected"),
		TEXT("Com_NonSelectedTexture"), (CComponent**)&m_pNonSelecetedTexture)))
		return E_FAIL;

	/* Com_SelectedTexture*/
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_Selected"),
		TEXT("Com_SelectedTexture"), (CComponent**)&m_pSelecetedTexture)))
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

HRESULT CUI_Logo_SelectBase::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	
	if (m_eMouseState == CUI::MOUSE_END)
	{
		if (FAILED(m_pNonSelecetedTexture->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;

	}
	else
	{		
		if (FAILED(m_pSelecetedTexture->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}
	

	return S_OK;
}

CUI_Logo_SelectBase * CUI_Logo_SelectBase::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Logo_SelectBase*	pInstance = new CUI_Logo_SelectBase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Logo_SelectBase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Logo_SelectBase::Clone(void* pArg)
{
	CUI_Logo_SelectBase*	pInstance = new CUI_Logo_SelectBase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Logo_SelectBase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Logo_SelectBase::Free()
{
	__super::Free();
}

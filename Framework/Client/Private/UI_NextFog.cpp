#include "stdafx.h"
#include "UI_NextFog.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Utils.h"

CUI_NextFog::CUI_NextFog(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext, L"UI_Next_Fog")
{

}

CUI_NextFog::CUI_NextFog(const CUI_NextFog & rhs)
	: CUI(rhs)
{
	
}

HRESULT CUI_NextFog::Initialize_Prototype(const UI_INFO& tInfo)
{
	m_tInfo = tInfo;

	return S_OK;
}

HRESULT CUI_NextFog::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		NEXT_INFO* pInfo = (NEXT_INFO*)pArg;
		m_eNextLevel = pInfo->eNextLevel;
		m_strDataFolderName = CUtils::ToWString(pInfo->strFolderName);
	}
	else
		return E_FAIL;
	
	m_pTransformCom->Set_Scale(XMLoadFloat3(&_float3(m_tInfo.fCX, m_tInfo.fCY, 1.f)));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_tInfo.fX - g_iWinSizeX * 0.5f, -m_tInfo.fY + g_iWinSizeY * 0.5f, 0.01f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));


	m_fAccTime = 0.f;
	m_iTextureIndex = 0;

	GI->Play_Sound(L"NextFog.wav", CHANNELID::SOUND_UI, 1.f);
	return S_OK;
}

void CUI_NextFog::Tick(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;
	if (m_fAccTime >= m_fNextTime)
	{
		m_fAccTime = 0.f;
		m_iTextureIndex++;
		
		if (m_iTextureIndex >= m_pTextureCom->Get_TextureCount())
		{
			// TODO Next.
			if (LEVELID::LEVEL_END != m_eNextLevel)
			{
				if (FAILED(GI->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, m_eNextLevel, m_strDataFolderName))))
					MSG_BOX("Open Level Failde. : CUI_NextFog");
			}
			// m_bDead = true;
			m_iTextureIndex = 0;
		}
	}
}

void CUI_NextFog::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RENDER_UI, this);
}

HRESULT CUI_NextFog::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();
	

	return S_OK;
}

HRESULT CUI_NextFog::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NextFog"),
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

HRESULT CUI_NextFog::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TransPose())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_AlphaTexture", m_iTextureIndex)))
		return E_FAIL;

	return S_OK;
}

CUI_NextFog * CUI_NextFog::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const UI_INFO& tInfo)
{
	CUI_NextFog*	pInstance = new CUI_NextFog(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(tInfo)))
	{
		MSG_BOX("Failed to Created : CUI_NextFog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_NextFog::Clone(void* pArg)
{
	CUI_NextFog*	pInstance = new CUI_NextFog(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_NextFog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_NextFog::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);

}

#include "..\Public\RenderTarget.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"

CRenderTarget::CRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vColor)
{
	m_vColor = vColor;

	D3D11_TEXTURE2D_DESC		TextureDesc = {};

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vColor);

	return S_OK;
}

HRESULT CRenderTarget::Bind_SRV(CShader * pShader, const _char * pConstantName)
{
	return pShader->Bind_Texture(pConstantName, m_pSRV);	
}


#ifdef _DEBUG

HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	D3D11_VIEWPORT		ViewportDesc;

	_uint				iNumViewports = 1;

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = fSizeX;
	m_WorldMatrix._22 = fSizeY;

	m_WorldMatrix._41 = fX - ViewportDesc.Width * 0.5f;
	m_WorldMatrix._42 = -fY + ViewportDesc.Height * 0.5f;

	return S_OK;
}



HRESULT CRenderTarget::Render(CShader * pShader, CVIBuffer_Rect * pBuffer)
{
	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(pShader->Bind_Texture("g_Texture", m_pSRV)))
		return E_FAIL;

	if (FAILED(pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(pBuffer->Render()))
		return E_FAIL;

	return S_OK;
}


#endif


CRenderTarget * CRenderTarget::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vColor)
{
	CRenderTarget*	pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iSizeX, iSizeY, ePixelFormat, vColor)))
	{
		MSG_BOX("Failed to Created : CRenderTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	// SaveDDSTextureToFile(m_pContext, m_pTexture2D, TEXT("../Bin/Diffuse.dds"));

	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);
	Safe_Release(m_pTexture2D);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

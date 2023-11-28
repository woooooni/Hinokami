#include "..\Public\Target_Manager.h"
#include "RenderTarget.h"

IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{

}



HRESULT CTarget_Manager::Ready_Shadow_DSV(ID3D11Device* pDevice, _uint iWinSizeX, _uint iWinSizeY)
{
	if (nullptr == pDevice)
		return E_FAIL;

	ID3D11Texture2D* pDSV = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWinSizeX * 10.f;
	TextureDesc.Height = iWinSizeY * 10.f;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDSV)))
		return E_FAIL;


	if (FAILED(pDevice->CreateDepthStencilView(pDSV, nullptr, &m_pShadowDSV)))
		return E_FAIL;

	Safe_Release(pDSV);

	return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vColor)
{
	if (nullptr != Find_RenderTarget(strTargetTag))
		return E_FAIL;

	CRenderTarget*		pRenderTarget = CRenderTarget::Create(pDevice, pContext, iSizeX, iSizeY, ePixelFormat, vColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const wstring & strMRTTag, const wstring & strTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>*	pMRTList = Find_MRT(strMRTTag);

	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>		MRTList;
		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(strMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return	S_OK;
}

HRESULT CTarget_Manager::Bind_SRV(CShader * pShader, const wstring & strTargetTag, const _char * pConstantName)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_SRV(pShader, pConstantName);
}

HRESULT CTarget_Manager::Begin_MRT(ID3D11DeviceContext* pContext, const wstring & strMRTTag, _bool bClear)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(strMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;
		
	pContext->OMGetRenderTargets(8, m_pPrevRTVs, &m_pDSV);

	ID3D11RenderTargetView*		pRenderTargets[8] = {};

	_uint			iNumRTVs = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTargets[iNumRTVs++] = pRenderTarget->Get_RTV();
		if(bClear)
			pRenderTarget->Clear();
	}

	pContext->OMSetRenderTargets(iNumRTVs, pRenderTargets, m_pDSV);

	return	S_OK;
}

HRESULT CTarget_Manager::Begin_Shadow_MRT(ID3D11DeviceContext* pContext, const wstring& strMRTTag)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	pContext->OMGetRenderTargets(8, m_pPrevRTVs, &m_pDSV);

	ID3D11RenderTargetView* pRenderTargets[8] = {};

	_uint			iNumRTVs = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTargets[iNumRTVs++] = pRenderTarget->Get_RTV();
		pRenderTarget->Clear();
	}

	pContext->ClearDepthStencilView(m_pShadowDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	pContext->OMSetRenderTargets(iNumRTVs, pRenderTargets, m_pShadowDSV);

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = 1600.f * 10.f;
	ViewPortDesc.Height = 900.f * 10.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	pContext->RSSetViewports(1, &ViewPortDesc);

	return	S_OK;
}

HRESULT CTarget_Manager::Clear_RenderTarget(const wstring& strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;
	
	return pRenderTarget->Clear();
}

HRESULT CTarget_Manager::End_MRT(ID3D11DeviceContext* pContext)
{

	pContext->OMSetRenderTargets(8, m_pPrevRTVs, m_pDSV);

	for (_uint i = 0; i < 8; ++i)	
		Safe_Release(m_pPrevRTVs[i]);

	Safe_Release(m_pDSV);

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = 1600.f;
	ViewPortDesc.Height = 900.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	pContext->RSSetViewports(1, &ViewPortDesc);

	return	S_OK;
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_Debug(const wstring & strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(strTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);	
}

HRESULT CTarget_Manager::Render(const wstring & strMRTTag, CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Render(pShader, pVIBuffer);
	}

	return S_OK;
}

#endif


CRenderTarget * CTarget_Manager::Find_RenderTarget(const wstring & strTargetTag)
{
	auto	iter = m_RenderTargets.find(strTargetTag);

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const wstring & strMRTTag)
{
	auto	iter = m_MRTs.find(strMRTTag);

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
		{
			Safe_Release(pRenderTarget);
		}
		Pair.second.clear();
	}
	m_MRTs.clear();
		

	for (auto& Pair : m_RenderTargets)	
		Safe_Release(Pair.second);

	m_RenderTargets.clear();
}

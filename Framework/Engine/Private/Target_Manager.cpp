#include "..\Public\Target_Manager.h"
#include "RenderTarget.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include <random>

IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{
	ZeroMemory(&m_vPlayerPos, sizeof(_float3));
}



void CTarget_Manager::Set_WipsData(_float* pWispRatios, _float4* pWispPoss, _int pWispNum)
{
	if (nullptr == pWispRatios || nullptr == pWispPoss)
	{
		m_bIsWisp = false;
		m_fMaxRatio = 0.f;
		return;
	}

	m_bIsWisp = true;
	m_iWispNum = pWispNum;
	memcpy(m_WispRatios, pWispRatios, sizeof(_float) * 256);
	memcpy(m_WispPoss, pWispPoss, sizeof(_float4) * 256);

	if (m_fMaxRatio < m_WispRatios[0])
		m_fMaxRatio = m_WispRatios[0];
}

void CTarget_Manager::Get_WispData(_float ** Out_pWispRatios, _float4 ** Out_pWispPoss, _bool* Out_bIsWisp, _int* Out_iWispNum)
{
	*Out_bIsWisp = m_bIsWisp;
	if (!m_bIsWisp)
	{
		*Out_iWispNum = 0;
		*Out_pWispRatios = nullptr;
		*Out_pWispPoss = nullptr;
	}
	else
	{
		*Out_iWispNum = m_iWispNum;
		*Out_pWispRatios = m_WispRatios;
		*Out_pWispPoss = m_WispPoss;
	}
}


HRESULT CTarget_Manager::Ready_ShadowDepthStencilRenderTargetView(ID3D11Device * pDevice, _uint iWinCX, _uint iWinCY)
{
	if (nullptr == pDevice)
		return E_FAIL;

	ID3D11Texture2D*		pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWinCX;
	TextureDesc.Height = iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;


	if (FAILED(pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDeptheStencil)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4 * pClearColor)
{
	if (nullptr != Find_RenderTarget(strTargetTag))
		return E_FAIL;

	CRenderTarget*		pRenderTarget = CRenderTarget::Create(pDevice, pContext, iSizeX, iSizeY, eFormat, pClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>*		pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>		MRTList;

		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(strMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Bind_SRV(const wstring& strTargetTag, CShader * pShader, const char * pConstantName)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_Texture(pShader, pConstantName);
}

HRESULT CTarget_Manager::Begin_MRT(ID3D11DeviceContext * pContext, const wstring& strMRTTag, _bool bClear)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;


	// 끼워져 있는 8개의 랜더 타겟와 스텐실 하나를 보관한다.
	_uint		iNumViews = 8;
	pContext->OMGetRenderTargets(iNumViews, m_pOldRenderTargets, &m_pOldDepthStencil);

	_uint			iNumRTVs = 0;

	ID3D11RenderTargetView*			RTVs[8] = { nullptr };

	for (auto& pRenderTarget : *pMRTList)
	{
		if(bClear)
			pRenderTarget->Clear();
		RTVs[iNumRTVs++] = pRenderTarget->Get_RTV();
	}



	pContext->OMSetRenderTargets(iNumRTVs, RTVs, m_pOldDepthStencil);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_ShadowMRT(ID3D11DeviceContext * pContext, const wstring& strMRTTag)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;


	// 끼워져 있는 8개의 랜더 타겟와 스텐실 하나를 보관한다.
	_uint		iNumViews = 8;
	pContext->OMGetRenderTargets(iNumViews, m_pOldRenderTargets, &m_pOldDepthStencil);

	_uint			iNumRTVs = 0;

	ID3D11RenderTargetView*			RTVs[8] = { nullptr };

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		RTVs[iNumRTVs++] = pRenderTarget->Get_RTV();
	}


	pContext->ClearDepthStencilView(m_pShadowDeptheStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	pContext->OMSetRenderTargets(iNumRTVs, RTVs, m_pShadowDeptheStencil);
	// pContext->OMSetRenderTargets(iNumRTVs, RTVs, m_pOldDepthStencil);

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = 1280.f * 12.5f;
	ViewPortDesc.Height = 720.f * 12.5f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	pContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT(ID3D11DeviceContext * pContext)
{
	_uint		iNumRTVs = 8;

	pContext->OMSetRenderTargets(iNumRTVs, m_pOldRenderTargets, m_pOldDepthStencil);

	for (_uint i = 0; i < 8; ++i)
		Safe_Release(m_pOldRenderTargets[i]);

	Safe_Release(m_pOldDepthStencil);

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = 1280.f;
	ViewPortDesc.Height = 720.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	pContext->RSSetViewports(1, &ViewPortDesc);


	return S_OK;
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Initialize_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(strTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Initialize_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CTarget_Manager::Render_Debug(const wstring& strMRTTag, class CVIBuffer* pVIBuffer, class CShader* pShader)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;


	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Render_Debug(pShader, pVIBuffer);
	}

	return S_OK;
}

#endif // _DEBUG

CRenderTarget * CTarget_Manager::Find_RenderTarget(const wstring& strTargetTag)
{
	auto	iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTag_Finder(strTargetTag.c_str()));

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const wstring& strMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTag_Finder(strMRTTag.c_str()));
	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free()
{

	Safe_Release(m_pShadowDeptheStencil);

	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);

		Pair.second.clear();
	}

	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);

	m_RenderTargets.clear();


}





_float CTarget_Manager::Get_RendomNum(_float fMin, _float fMax)
{
	// 시드값을 얻기 위한 random_device 생성.
	std::random_device rd;

	// random_device 를 통해 난수 생성 엔진을 초기화 한다.
	std::mt19937 gen(rd());

	// 0 부터 99 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
	std::uniform_real_distribution<_float> dis(fMin, fMax);
	return dis(gen);
}

_int CTarget_Manager::Get_RendomNum_Int(_int iMin, _int iMax)
{
	// 시드값을 얻기 위한 random_device 생성.
	std::random_device rd;

	// random_device 를 통해 난수 생성 엔진을 초기화 한다.
	std::mt19937 gen(rd());

	// 0 부터 99 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
	std::uniform_int_distribution<_int> dis(iMin, iMax);
	return dis(gen);
}


#include "..\Public\Target_Manager.h"
#include "RenderTarget.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{
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
	if(nullptr == pRenderTarget)
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

	return pRenderTarget->Bind_SRV(pShader, pConstantName);	
}

HRESULT CTarget_Manager::Begin_MRT(ID3D11DeviceContext * pContext, const wstring& strMRTTag)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	_uint		iNumViews = 8;
	pContext->OMGetRenderTargets(iNumViews, m_pOldRenderTargets, &m_pOldDepthStencil);
		
	_uint			iNumRTVs = 0;

	ID3D11RenderTargetView*			RTVs[8] = { nullptr };

	for (auto& pRenderTarget : *pMRTList)
	{		
		pRenderTarget->Clear();
		RTVs[iNumRTVs++] = pRenderTarget->Get_RTV();
	}

		

	pContext->OMSetRenderTargets(iNumRTVs, RTVs, m_pOldDepthStencil);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT(ID3D11DeviceContext * pContext)
{
	_uint		iNumRTVs = 8;	

	pContext->OMSetRenderTargets(iNumRTVs, m_pOldRenderTargets, m_pOldDepthStencil);

	for (_uint i = 0; i < 8; ++i)
		Safe_Release(m_pOldRenderTargets[i]);

	Safe_Release(m_pOldDepthStencil);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Initialize_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget*		pRenderTarget =  Find_RenderTarget(strTargetTag);

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
	auto	iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTag_Finder(strTargetTag));

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;	
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const wstring& strMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTag_Finder(strMRTTag));
	if(iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free()
{


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

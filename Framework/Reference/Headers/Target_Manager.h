#pragma once

/* ����Ÿ�ٵ��� �����ؼ� ��Ƶд�. */
#include "Base.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)
private:
	CTarget_Manager();
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Ready_Shadow_DSV(ID3D11Device* pDevice, _uint iWinSizeX, _uint iWinSizeY);

	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);
	
	HRESULT Bind_SRV(class CShader* pShader, const wstring& strTargetTag, const _char* pConstantName);

	/* strMRTTag�� �ش��ϴ� list�� ����ִ� Ÿ�ٵ��� ��ġ�� ���ε��Ѵ�. */
	HRESULT Begin_MRT(ID3D11DeviceContext* pContext, const wstring& strMRTTag, _bool bClear = true);
	HRESULT Begin_Shadow_MRT(ID3D11DeviceContext* pContext, const wstring& strMRTTag);



	HRESULT Clear_RenderTarget(const wstring& strTargetTag);

	/* �ٽ� ���� ���·� �����Ѵ�. */
	HRESULT End_MRT(ID3D11DeviceContext* pContext);



#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

private:
	map<const wstring, class CRenderTarget*>			m_RenderTargets;	

	/* ��ġ�� ���ÿ� ���ε��Ǿ���ϴ� Ÿ�ٵ��� �̸� ����ΰڴ�. */
	map<const wstring, list<class CRenderTarget*>>		m_MRTs;

private:
	// ID3D11RenderTargetView*					m_pBackBufferRTV = { nullptr };


	ID3D11RenderTargetView*					m_pPrevRTVs[8];
	ID3D11DepthStencilView*					m_pDSV = { nullptr };
	ID3D11DepthStencilView*					m_pShadowDSV = { nullptr };

private:
	class CRenderTarget* Find_RenderTarget(const wstring& strTargetTag);
	list<class CRenderTarget*>* Find_MRT(const wstring & strMRTTag);


public:
	virtual void Free() override;
};

END
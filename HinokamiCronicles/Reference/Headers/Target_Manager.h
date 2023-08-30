#pragma once

#include "Base.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)
public:
	CTarget_Manager();
	virtual ~CTarget_Manager() = default;

public:	
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4* pClearColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);

public:
	HRESULT Bind_SRV(const wstring& strTargetTag, class CShader* pShader, const char* pConstantName);

	/* 기존에 있던 렌더타겟을 빼고, 지정한 렌더타겟들(mrt)을 장치에 순서대로 바인딩한다. */
	HRESULT Begin_MRT(ID3D11DeviceContext* pContext, const wstring& strMRTTag);

	/* 원래 상태로 복구한다.(BackBuffer를 장치에 셋한다.) */
	HRESULT End_MRT(ID3D11DeviceContext* pContext);

#ifdef _DEBUG
public:	
	HRESULT Initialize_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(const wstring& strMRTTag, class CVIBuffer* pVIBuffer, class CShader* pShader);
#endif // _DEBUG


private: /* 생성한 렌더타겟들을 전체 다 모아놓는다. */
	map<const wstring, class CRenderTarget*>			m_RenderTargets;
	typedef map<const wstring, class CRenderTarget*>	RENDERTARGETS;

private: /* 동시에 바인딩되어야할 렌더타겟들을 LIST로 묶어놓는다. (Diffuse + Normal + Depth, Shader + Specular) */
	map<const wstring, list<class CRenderTarget*>>			m_MRTs;
	typedef map<const wstring, list<class CRenderTarget*>>	MRTS;

private:
	ID3D11RenderTargetView*					m_pOldRenderTargets[8] = { nullptr };
	ID3D11DepthStencilView*					m_pOldDepthStencil = nullptr;



private:
	class CRenderTarget* Find_RenderTarget(const wstring& strTargetTag);
	list<class CRenderTarget*>* Find_MRT(const wstring& strMRTTag);

public:
	virtual void Free() override;
};

END
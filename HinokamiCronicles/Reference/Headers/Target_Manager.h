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

	/* ������ �ִ� ����Ÿ���� ����, ������ ����Ÿ�ٵ�(mrt)�� ��ġ�� ������� ���ε��Ѵ�. */
	HRESULT Begin_MRT(ID3D11DeviceContext* pContext, const wstring& strMRTTag);

	/* ���� ���·� �����Ѵ�.(BackBuffer�� ��ġ�� ���Ѵ�.) */
	HRESULT End_MRT(ID3D11DeviceContext* pContext);

#ifdef _DEBUG
public:	
	HRESULT Initialize_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(const wstring& strMRTTag, class CVIBuffer* pVIBuffer, class CShader* pShader);
#endif // _DEBUG


private: /* ������ ����Ÿ�ٵ��� ��ü �� ��Ƴ��´�. */
	map<const wstring, class CRenderTarget*>			m_RenderTargets;
	typedef map<const wstring, class CRenderTarget*>	RENDERTARGETS;

private: /* ���ÿ� ���ε��Ǿ���� ����Ÿ�ٵ��� LIST�� ������´�. (Diffuse + Normal + Depth, Shader + Specular) */
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
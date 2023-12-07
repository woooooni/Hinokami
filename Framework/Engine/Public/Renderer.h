#pragma once

#include "Component.h"

/* 1. 화면에 그려져야하는 객체들을 그리는 순서대로 모아서 보관한다. */
/* 2. 보관하고 있는 객체들의 렌더콜(드로우콜)을 수행한다. */

BEGIN(Engine)
class CVIBuffer_Instancing;
class ENGINE_DLL CRenderer final : public CComponent
{
public:
	typedef struct tagEffectInstancingDesc
	{
		_int		g_iCutUV;
		_float		g_fMaxCountX;
		_float		g_fMaxCountY;
		_float		g_fAlpha;
		_float2		g_fUVIndex;
		_float2		g_fUVFlow;

		_float4		g_fAdditiveDiffuseColor;
		_float4		g_vBloomPower;

	} EFFECT_INSTANCE_DESC;

public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_SHADOW, RENDER_NONLIGHT, RENDER_LIGHT, RENDER_NONBLEND, RENDER_ALPHABLEND, RENDER_EFFECT, RENDER_UI, RENDER_END };
	enum SHADER_TYPE { MODEL, RECT, EFFECT_TEXTURE, EFFECT_MODEL, TYPE_END };
	
public:
	typedef struct tagTextDesc
	{
		wstring strText;
		wstring strFontTag;
		_float2 vPosition;
		_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
		_float fAngle = 0.f;
		_float2 vOrigin = { 0.f, 0.f };
		_float2 vScale = { 1.f, 1.f };
	} TEXT_DESC;

private:
	typedef struct tagInstancingDesc
	{
		class CGameObject* pGameObject = { nullptr };
		vector<XMFLOAT4X4> WorldMatrices;
		vector<EFFECT_INSTANCE_DESC> EffectInstancingDesc;
		SHADER_TYPE eShaderType = SHADER_TYPE::TYPE_END;
	}INSTANCING_DESC;

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRenderer(const CRenderer& rhs) = delete;
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Add_RenderGroup_Instancing(RENDERGROUP eRenderGroup, SHADER_TYPE eShaderType, class CGameObject* pGameObject, _float4x4 WorldMatrix);
	HRESULT Add_RenderGroup_Instancing_Effect(RENDERGROUP eRenderGroup, SHADER_TYPE eShaderType, class CGameObject* pGameObject, _float4x4 WorldMatrix, const EFFECT_INSTANCE_DESC& EffectInstanceDesc);
	HRESULT Draw();


#ifdef _DEBUG
public:
	HRESULT Add_Debug(class CComponent* pDebug) {
		/*m_RenderDebug.push_back(pDebug);
		Safe_AddRef(pDebug);*/
		return S_OK;
	}

	HRESULT Add_Text(const TEXT_DESC & TextDesc) {
		m_RenderTexts.push_back(TextDesc);
		return S_OK;
	}

	void Set_PlayerPosition(_vector vPosition) { XMStoreFloat4(&m_vPlayerPosition, vPosition); }

#endif

public:
	void Set_FogColor(_float4 vFogColor) { m_vFogColor = vFogColor; }
	void Set_FogStartEnd(_float2 vFogStartEnd) { m_vFogStartEnd = vFogStartEnd; }

private:
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonLight();
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_Lights();
	HRESULT Render_Deferred();

	HRESULT Render_AlphaBlend();
	HRESULT Render_Effect();


	


	///////////////////// Blur
	HRESULT Render_Blur(const wstring& strStartTargetTag, const wstring& strFinalMrtTag, _bool bClear);
	HRESULT Render_BlurDownSample(const wstring& strStartTargetTag);
	HRESULT Render_Blur_Horizontal();
	HRESULT Render_Blur_Vertical();
	HRESULT Render_BlurUpSample(const wstring& strFinalMrtTag, _bool bClear);
	//////////////////////

	///////////////////// Bloom
	// HRESULT Render_Bloom();
	/////////////////////

	HRESULT Render_Final();
	HRESULT Render_UI();
	HRESULT Render_Text();

private:
	HRESULT Render_Debug();

private:
	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	class CShader* m_pShader = { nullptr };

	class CTarget_Manager* m_pTarget_Manager = { nullptr };
	class CLight_Manager* m_pLight_Manager = { nullptr };

	_float4x4					m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;


private:
	_float4						m_vFogColor = {.5f, .5f, .5f, 1.f };
	_float2						m_vFogStartEnd = { 1000000.f, 1000000.f };

private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	list<class CComponent*>				m_RenderDebug;

	list<TEXT_DESC>						m_RenderTexts;
	// Instancing
	class CVIBuffer_Instancing*			m_pVIBuffer_Instancing = nullptr;
	class CShader*						m_pIntancingShaders[SHADER_TYPE::TYPE_END];
	map<wstring, INSTANCING_DESC>		m_Render_Instancing_Objects[RENDER_END];

	_float								m_fBias = 0.2f;
	_float4								m_vPlayerPosition = {0.f, 0.f, 0.f, 1.f};
	_bool m_bDebugDraw = false;


	
public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
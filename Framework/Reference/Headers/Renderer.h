#pragma once

#include "Component.h"

/* 1. 화면에 그려져야하는 객체들을 그리는 순서대로 모아서 보관한다. */
/* 2. 보관하고 있는 객체들의 렌더콜(드로우콜)을 수행한다. */

BEGIN(Engine)
class CVIBuffer_Instancing;
class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_SHADOW, RENDER_NONLIGHT, RENDER_LIGHT, RENDER_NONBLEND, RENDER_ALPHABLEND, RENDER_EFFECT, RENDER_UI, RENDER_END };
	enum SHADER_TYPE { MODEL, RECT, EFFECT_TEXTURE, EFFECT_MODEL, TYPE_END };

private:
	typedef struct tagInstancingDesc
	{
		class CGameObject* pGameObject = { nullptr };
		vector<XMFLOAT4X4> WorldMatrices;
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
	HRESULT Draw();


public:
	HRESULT Add_Debug(class CComponent* pDebug) {
		m_RenderDebug.push_back(pDebug);
		Safe_AddRef(pDebug);
		return S_OK;
	}



private:
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonLight();
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_Lights();
	HRESULT Render_Deferred();


	HRESULT Render_AlphaBlend();
	HRESULT Render_Effect();

	HRESULT Render_BlurDownSample();
	HRESULT Render_BlurXY();
	HRESULT Render_BlurUpSample();
	HRESULT Render_MRT_Final();
	HRESULT Render_UI();

private:
	HRESULT Render_Debug();

private:
	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	class CShader* m_pShader = { nullptr };

	class CTarget_Manager* m_pTarget_Manager = { nullptr };
	class CLight_Manager* m_pLight_Manager = { nullptr };

	_float4x4					m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;


private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	list<class CComponent*>				m_RenderDebug;


	// Instancing
	class CVIBuffer_Instancing*			m_pVIBuffer_Instancing = nullptr;
	class CShader*						m_pIntancingShaders[SHADER_TYPE::TYPE_END];
	map<wstring, INSTANCING_DESC>		m_Render_Instancing_Objects[RENDER_END];


	
public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
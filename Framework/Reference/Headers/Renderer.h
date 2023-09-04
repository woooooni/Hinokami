#pragma once

#include "Component.h"

/* 1. 화면에 그려져야하는 객체들을 그리는 순서대로 모아서 보관한다. */
/* 2. 보관하고 있는 객체들의 렌더콜(드로우콜)을 수행한다. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONLIGHT, RENDER_LIGHT, RENDER_NONBLEND, RENDER_BLEND, RENDER_ALPHABLEND, RENDER_UI, RENDER_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	CRenderer(const CRenderer& rhs) = delete;
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw();

private:
	class CShader* m_pShader = { nullptr };


private:
	HRESULT Render_Priority();
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_Lights();
	HRESULT Render_Blend();
	HRESULT Render_NonLight();
	HRESULT Render_AlphaBlend();
	HRESULT Render_UI();

private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
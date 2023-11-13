#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CNavigation;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
protected:
	/* 원형을 생성할 때 */
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CTerrain(const CGameObject& rhs); /* 복사 생성자. */

	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	CShader* Get_ShaderCom() { return m_pShaderCom; }
	CTransform* Get_TransformCom() { return m_pTransformCom; }
	CVIBuffer_Terrain* Get_TerrainBufferCom() { return m_pVIBufferCom; }
	CNavigation* Get_NavigationCom() { return m_pNavigationCom; }


#ifdef _DEBUG
public:
	void Set_DrawGrid(_bool bDraw) { m_bDraw = bDraw; }
	_bool Is_DrawGrid() { return m_bDraw; }

protected:
	class PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	class BasicEffect* m_pEffect = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;
	_float4	m_vColor = _float4(0.f, 1.f, 0.f, 1.f);
	_bool m_bDraw = true;
#endif



private: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CRenderer*				m_pRendererCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };		
	CNavigation*			m_pNavigationCom = { nullptr };
	CVIBuffer_Terrain*		m_pVIBufferCom = { nullptr };



private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END
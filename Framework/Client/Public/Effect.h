#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Model.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CPipeLine;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect abstract : public CGameObject
{
public:
	typedef struct tagTextureEffectDesc
	{
		
		_float2			vAccUV			= _float2(0.f, 0.f);
		_float2			fUVIndex		= _float2(0.f, 0.f);

		_bool			bIncrement = true;

		_uint			iMaxCountX		= 0.f;
		_uint			iMaxCountY		= 0.f;

		_float			fAccUVIndex		= 0.f;
		_float			fNextIndexSpeed = 0.f;
	} TEXTURE_EFFECT_DESC;

	typedef struct tagMeshEffectDesc
	{
		_float2 vUVAcc = _float2(0.f, 0.f);
		_float2 vUVSpeed = _float2(0.f, 0.f);

	} MESH_EFFECT_DESC;

	enum EFFECT_TYPE
	{
		EFFECT_TEXTURE,
		EFFECT_MESH,
		EFFECT_END
	};
protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, EFFECT_TYPE eType);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) PURE;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) PURE;
	virtual HRESULT Render() PURE;

protected: /* For.Texture_Effect */
	virtual void IncrementUV(_float fTimeDelta);
	virtual void DecrementUV(_float fTimeDelta);

protected: /* For.Mesh_Effect */
	virtual void Update_MeshUV(_float fTimeDelta);

protected:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT Ready_Components() override;

public:
	const TEXTURE_EFFECT_DESC& Get_Texutre_EffectDesc() { return m_tTextureEffectDesc; }
	void Set_Texture_EffectDesc(const TEXTURE_EFFECT_DESC& tEffectDesc) { m_tTextureEffectDesc = tEffectDesc; }

	const MESH_EFFECT_DESC& Get_Mesh_EffectDesc() { return m_tMeshEffectDesc; }
	void Set_Mesh_EffectDesc(const MESH_EFFECT_DESC& tEffectDesc) { m_tMeshEffectDesc = tEffectDesc; }

	CTexture* Get_Texture() { return m_pEffectTexture; }

protected:
	class CShader* m_pShaderCom = nullptr;
	class CRenderer* m_pRendererCom = nullptr;
	class CTransform* m_pTransformCom = nullptr;
	class CTexture* m_pEffectTexture = nullptr;

protected:
	TEXTURE_EFFECT_DESC m_tTextureEffectDesc;
	MESH_EFFECT_DESC m_tMeshEffectDesc;

protected:
	EFFECT_TYPE m_eType = EFFECT_TYPE::EFFECT_END;

protected:
	_bool			m_bEnd = false;
	_bool			m_bLoop = true;

public:
	virtual void Free() override;

};

END


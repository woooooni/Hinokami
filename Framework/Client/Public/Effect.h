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
	typedef struct tagEffectDesc
	{
		_float4			vDir = _float4(0.f, 0.f, 0.f, 0.f);
		_float4			vRotationDir = _float4(0.f, 0.f, 0.f, 0.f);

		_float2			vAccUV = _float2(0.f, 0.f);
		_float			fUSpeed = 0.f;
		_float			fVSpeed = 0.f;
		_uint			iTextureIndex = 0;
	} EFFECT_DESC;

protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) PURE;
	virtual void Tick(_float fTimeDelta) PURE;
	virtual void LateTick(_float fTimeDelta) PURE;
	virtual HRESULT Render() PURE;

protected:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT Ready_Components() override;

public:
	const EFFECT_DESC& Get_EffectDesc() { return m_tEffectDesc; }
	void Set_EffectDesc(const EFFECT_DESC& tEffectDesc) { m_tEffectDesc = tEffectDesc; }

	CTexture* Get_Texture() { return m_pEffectTexture; }

protected:
	class CShader* m_pShaderCom = nullptr;
	class CRenderer* m_pRendererCom = nullptr;
	class CTransform* m_pTransformCom = nullptr;
	class CTexture* m_pEffectTexture = nullptr;

protected:
	EFFECT_DESC m_tEffectDesc;

public:
	virtual void Free() override;

};

END


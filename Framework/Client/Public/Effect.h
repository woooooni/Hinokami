#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CPipeLine;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect final : public CGameObject
{
public:
	typedef struct tagEffectDesc
	{	
		_int iDiffuseTextureIndex = -1;
		_int iAlphaTextureIndex = -1;

		_float			fTurnSpeed = 0.f;
		_float			fMoveSpeed = 0.f;

		// Sequencetexture Effect
		_float			fMaxCountX		= 1.f;
		_float			fMaxCountY		= 1.f;
		
		_float			fAlpha			= 1.f;

		_float			fAccIndex = 0.f;
		_float			fIndexSpeed = 20.f;

		_float2			fUVIndex = _float2(0.f, 0.f);
		_float3			fAdditiveDiffuseColor	= _float3(0.f, 0.f, 0.f);


		
		// Translation
		_float4x4		OffsetMatrix; 
		_float3			vMoveDir = _float3(0.f, 0.f, 0.f);
		_float3			vTurnDir = _float3(0.f, 1.f, 0.f);

		tagEffectDesc()
		{
			XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
		}

	} EFFECT_DESC;

	enum EFFECT_TYPE
	{
		EFFECT_TEXTURE,
		EFFECT_MESH,
		EFFECT_END
	};


protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, EFFECT_TYPE eType, const wstring& strPrototypeModelName);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;


public:
	virtual HRESULT Initialize_Prototype(const EFFECT_DESC& tEffectDesc);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT Save_EffectInfo();


public:
	const EFFECT_DESC& Get_EffectDesc() { return m_tEffectDesc; }
	void Set_EffectDesc(const EFFECT_DESC& tDesc) { memcpy(&m_tEffectDesc, &tDesc, sizeof(EFFECT_DESC)); }

	_bool Is_End() { return m_bEnd; };
	void Set_End(_bool bEnd) { m_bEnd = bEnd; }

	_bool Is_Loop() { return m_bLoop; }
	void Set_Loop(_bool bLoop) { m_bLoop = bLoop; }

	_bool Is_Increment() { return m_bIncrement; }
	void Set_Increment(_bool bIncrement) { m_bIncrement = bIncrement; }

	EFFECT_TYPE	Get_EffectType() { return m_eType; }

	const wstring& Get_EffectModelName() { return m_strModelPrototype; }

	void Set_OffsetMatrix(_fmatrix fOffsetMatrix) { XMStoreFloat4x4(&m_tEffectDesc.OffsetMatrix, fOffsetMatrix); }
	const _float4x4& Get_OffsetMatrix() { return  m_tEffectDesc.OffsetMatrix; }


public:
	class CTexture* Get_DiffuseTexture() { return m_pDiffuseTextureCom; }
	class CTexture* Get_AlphaTexture() { return m_pAlphaTextureCom; }
	class CTransform* Get_TransformCom() { return m_pTransformCom; }
	

private:
	class CModel* m_pModelCom = nullptr;
	class CShader* m_pShaderCom = nullptr;
	class CTexture* m_pDiffuseTextureCom = nullptr;
	class CTexture* m_pAlphaTextureCom = nullptr;
	class CRenderer* m_pRendererCom = nullptr;
	class CTransform* m_pTransformCom = nullptr;
	class CVIBuffer_Rect* m_pVIBufferCom = nullptr;

private:
	EFFECT_TYPE m_eType = EFFECT_TYPE::EFFECT_END;
	EFFECT_DESC m_tEffectDesc;

private:
	_bool			m_bEnd = false;
	_bool			m_bLoop = true;
	_bool			m_bIncrement = true;


private:
	wstring m_strModelPrototype = L"";
	_uint m_iPassIndex = 0;

private: /* For.Texture_Effect */
	virtual void Increment(_float fTimeDelta);
	virtual void Decrement(_float fTimeDelta);

private:
	HRESULT Bind_ShaderResource();

protected:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT Ready_Components() override;

public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
		const wstring& strObjectTag, EFFECT_TYPE eType, const wstring& strPrototypeModelName, const EFFECT_DESC& tEffectDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END



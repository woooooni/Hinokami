#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "VIBuffer_Trail.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CTransform;
END


BEGIN(Client)

class CTrail final : public CGameObject
{

public:
	typedef struct tagTrailDesc
	{
		_bool bTrail = false;

		_float fAccGenTrail = 0.f;
		_float fGenTrailTime = 0.01f;

		_float4 vDiffuseColor = { 0.f, 0.f, 0.f, 1.f };

		_float2 vUVAcc = { 0.f, 0.f };
		_float2 vUV_FlowSpeed = { 0.f, 0.f };

		_int bUV_Cut = 1;
	} TRAIL_DESC;

private:
	CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;

public:
	virtual HRESULT Initialize_Prototype(const TRAIL_DESC& TrailDesc);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void SetUp_Position(_vector vHighPosition, _vector vLowPosition);
	void Start_Trail(_matrix TransformMatrix);
	void Stop_Trail();


public:
	void Set_TransformMatrix(_matrix TransformMatrix)
	{
		XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);
	}

	void Set_Color(_float4 vColor) { m_TrailDesc.vDiffuseColor = vColor; }
	void Set_DiffuseTexture_Index(_int iIndex) { m_iDiffuseTextureIndex = iIndex; }
	void Set_AlphaTexture_Index(_int iIndex) { m_iAlphaTextureIndex = iIndex; }

	void Set_DiffuseTexture_Index(const wstring& strDiffuseTextureName);
	void Set_AlphaTexture_Index(const wstring& strAlphaTextureName);

	const TRAIL_DESC& Get_TrailDesc() { return m_TrailDesc; }
	void Set_TrailDesc(const TRAIL_DESC& TrailDesc) { m_TrailDesc = TrailDesc; }

	void Set_VtxCount(_uint iCount) { m_pVIBufferCom->Set_VtxCount(iCount); }
public:
	void Set_UV_Speed(_float2 vUVSpeed) { m_vTrailUVSpeed = vUVSpeed; }

private:
	virtual HRESULT Ready_Components() override;


private:
	_float4x4 m_TransformMatrix;

private:
	class CRenderer* m_pRendererCom = nullptr;
	class CShader* m_pShaderCom = nullptr;
	class CTexture* m_pDiffuseTextureCom = nullptr;
	class CTexture* m_pAlphaTextureCom = nullptr;
	class CVIBuffer_Trail* m_pVIBufferCom = nullptr;




private:
	_float2 m_vTrailUVSpeed = { 0.f, 0.f };

private:
	_int m_iDiffuseTextureIndex = -1;
	_int m_iAlphaTextureIndex = -1;

private:
	// 임시 저장용.
	TRAIL_DESC m_TrailDesc;

public:
	static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
		const wstring& strObjectTag, const TRAIL_DESC& TrailDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


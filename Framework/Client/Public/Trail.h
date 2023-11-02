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

private:
	CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Trail::TRAIL_DESC& TrailDesc);
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


private:
	virtual HRESULT Ready_Components() override;


private:
	_float4x4 m_TransformMatrix;

private:
	class CRenderer* m_pRendererCom = nullptr;
	class CShader* m_pShaderCom = nullptr;
	class CTexture* m_pTextureCom = nullptr;
	class CVIBuffer_Trail* m_pVIBufferCom = nullptr;




private:
	// 임시 저장용.
	CVIBuffer_Trail::TRAIL_DESC m_TrailDesc;

public:
	static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
		const wstring& strObjectTag, const CVIBuffer_Trail::TRAIL_DESC& TrailDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


#pragma once

#include "Client_Defines.h"
#include "VIBuffer.h"
#include <deque>
BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CVIBuffer_Trail final : public CVIBuffer
{
public:
	typedef struct tagTrailDesc
	{
		_bool bTrail = false;
		_float fAccGenTrail = 0.f;
		_float fGenTrailTime = 0.01f;
		_float4 vColor = {0.f, 0.f, 0.f, 0.5f};
		wstring strTextureName = L"";
		_float fUVAcc = 0.f;
	} TRAIL_DESC;

private:
	CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	void Update_Trail(_float fTimedelta, _matrix TransformMatrix);
	virtual HRESULT Render() override;

public:
	void SetUp_Position(_vector vHighPosition, _vector vLowPosition);
	void Start_Trail(_matrix TransformMatrix);
	void Stop_Trail();


private:
	TRAIL_DESC m_tTrailDesc;



private:
	_float4 m_vHighPosition = { 0.f, .5f, 0.f, 1.f };
	_float4 m_vLowPosition= { 0.f, -.5f, 0.f, 1.f };

	std::deque<VTXPOSTEX> m_TrailVertices;

	_uint m_iVtxCount = 0;

public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END
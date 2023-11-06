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


private:
	CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iVtxCount);
	CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	void Update_TrailBuffer(_float fTimedelta, _matrix TransformMatrix);
	virtual HRESULT Render() override;


public:
	void SetUp_Position(_vector vHighPosition, _vector vLowPosition);
	void Start_Trail(_matrix TransformMatrix);
	void Stop_Trail();

public:
	void Set_VtxCount(_uint iCount) { m_iVtxCount = iCount; }




private:
	_float4 m_vHighPosition = { 0.f, .5f, 0.f, 1.f };
	_float4 m_vLowPosition= { 0.f, -.5f, 0.f, 1.f };


	std::deque<VTXPOSTEX> m_TrailVertices;



	_uint m_iVtxCount = 0;
	_uint m_iEndIndex = 0;
	_uint m_iCatmullRomCount = 5;
	_uint m_iCatmullRomIndex[4] = {0, 0, 0, 0};

public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iVtxCount);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END
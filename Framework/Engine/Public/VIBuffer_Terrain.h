#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iSizeX, _uint iSizeZ);
	virtual HRESULT Initialize_Prototype(const wstring& strHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg) override;

public:
	_ulong			Get_VertexCount_X() { return m_iNumVerticesX; }
	_ulong			Get_VertexCount_Z() { return m_iNumVerticesZ; }

private:
	_ulong			m_iNumVerticesX = { 0 };
	_ulong			m_iNumVerticesZ = { 0 };

public:
	static CVIBuffer_Terrain* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strHeightMapFilePath);
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeZ);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

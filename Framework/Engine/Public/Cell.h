#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINTS { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
	typedef struct tagCellDesc
	{
		_bool				bMovable = true;
		_uint				iIndex = {};
	} CELL_DESC;

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	const _float3* Get_PointWorld(POINTS ePoint) {
		return &m_vPoints_InWorld[ePoint];
	}

	void SetUp_Neighbor(LINE eLine, CCell* pCell) {
		m_iNeighborIndices[eLine] = pCell->m_iIndex;
	}

public:
	HRESULT Initialize(const _float3* pPoints, _uint iIndex);
	HRESULT Initialize(const CELL_DESC& tDesc, const vector<_float3>& Points);
	void Update(_fmatrix WorldMatrix);
	_bool Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint);
	_bool isOut(_fvector vPoint, _fmatrix WorldMatrix, _int* pNeighborIndex);

public:
	void Set_Movable(_bool _bMovable) { m_bMovable = _bMovable; }
	_bool Is_Movable() { return m_bMovable; }
	_bool Is_HasNeighbor(){
		return m_iNeighborIndices[LINE_AB] != -1 || m_iNeighborIndices[LINE_BC] != -1 || m_iNeighborIndices[LINE_CA] != -1;
	}
	_bool Is_InCell(_vector vWorldPosition);

	_int Get_Index() { return m_iIndex; }


#ifdef _DEBUG
public:
	HRESULT Render();

#endif

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

#ifdef _DEBUG
private:
	class CVIBuffer_Cell*	m_pVIBuffer = { nullptr };
#endif

private:
	_uint				m_iIndex = {};

	_float3				m_vPoints_InWorld[POINT_END];
	_float3				m_vNormals[LINE_END];

	_int				m_iNeighborIndices[LINE_END] = { -1, -1, -1 };

	_bool				m_bMovable = true;


private:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex);
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CELL_DESC& tDesc, const vector<_float3>& Points);

public:
	virtual void Free() override;

	friend class CNavigation;
};

END
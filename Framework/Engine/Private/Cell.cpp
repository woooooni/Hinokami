#include "..\Public\Cell.h"
#include "VIBuffer_Cell.h"
#include <filesystem>
#include "FileUtils.h"
#include "GameInstance.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3 * pPoints, _uint iIndex)
{
	m_iIndex = iIndex;

	memcpy(m_vPoints_InWorld, pPoints, sizeof(_float3) * POINT_END);

	_float3		vLines[LINE_END];

	XMStoreFloat3(&vLines[LINE_AB], XMLoadFloat3(&m_vPoints_InWorld[POINT_B]) - XMLoadFloat3(&m_vPoints_InWorld[POINT_A]));
	XMStoreFloat3(&vLines[LINE_BC], XMLoadFloat3(&m_vPoints_InWorld[POINT_C]) - XMLoadFloat3(&m_vPoints_InWorld[POINT_B]));
	XMStoreFloat3(&vLines[LINE_CA], XMLoadFloat3(&m_vPoints_InWorld[POINT_A]) - XMLoadFloat3(&m_vPoints_InWorld[POINT_C]));

	for (size_t i = 0; i < LINE_END; i++)	
	{
		m_vNormals[i] = _float3(vLines[i].z * -1.f, 0.f, vLines[i].x);
		XMStoreFloat3(&m_vNormals[i], XMVector3Normalize(XMLoadFloat3(&m_vNormals[i])));
	}

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints_InWorld);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CCell::Initialize(const CELL_DESC& tDesc, vector<_float3>& Points)
{
	m_iIndex = tDesc.iIndex;
	m_bMovable = tDesc.bMovable;

	_vector vLineAB = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&Points[POINT_B]) - XMLoadFloat3(&Points[POINT_A]), 0.f));
	_vector vLineAC = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&Points[POINT_C]) - XMLoadFloat3(&Points[POINT_A]), 0.f));

	if (XMVectorGetY(XMVector3Cross(vLineAB, vLineAC)) < 0.f)
	{
		_float3 vTemp = Points[POINT_B];
		Points[POINT_B] = Points[POINT_C];
		Points[POINT_C] = vTemp;
	}


	memcpy(m_vPoints_InWorld, Points.data(), sizeof(_float3) * POINT_END);

	_float3		vLines[LINE_END];

	XMStoreFloat3(&vLines[LINE_AB], XMLoadFloat3(&m_vPoints_InWorld[POINT_B]) - XMLoadFloat3(&m_vPoints_InWorld[POINT_A]));
	XMStoreFloat3(&vLines[LINE_BC], XMLoadFloat3(&m_vPoints_InWorld[POINT_C]) - XMLoadFloat3(&m_vPoints_InWorld[POINT_B]));
	XMStoreFloat3(&vLines[LINE_CA], XMLoadFloat3(&m_vPoints_InWorld[POINT_A]) - XMLoadFloat3(&m_vPoints_InWorld[POINT_C]));

	for (size_t i = 0; i < LINE_END; i++)
	{
		m_vNormals[i] = _float3(vLines[i].z * -1.f, 0.f, vLines[i].x);

		XMStoreFloat3(&m_vNormals[i], XMVector3Normalize(XMLoadFloat3(&m_vNormals[i])));
	}

	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints_InWorld);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	return S_OK;
}

void CCell::Update(_fmatrix WorldMatrix)
{
	//for (size_t i = 0; i < POINT_END; i++)
	//{
	//	XMStoreFloat3(&m_vPoints_InWorld[i],
	//		XMVector3TransformCoord(XMLoadFloat3(&m_vPoints_InWorld[i]), WorldMatrix));
	//}

}

_bool CCell::Compare_Points(const _float3 * pSourPoint, const _float3 * pDestPoint)
{
	
	//_vector		vResult = XMVectorEqual(A, B);
	//XMVector3Equal( A, B);

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints_InWorld[POINT_A]), XMLoadFloat3(pSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints_InWorld[POINT_B]), XMLoadFloat3(pDestPoint)))
			return true;		

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints_InWorld[POINT_C]), XMLoadFloat3(pDestPoint)))
			return true;		
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints_InWorld[POINT_B]), XMLoadFloat3(pSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints_InWorld[POINT_A]), XMLoadFloat3(pDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints_InWorld[POINT_C]), XMLoadFloat3(pDestPoint)))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints_InWorld[POINT_C]), XMLoadFloat3(pSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints_InWorld[POINT_A]), XMLoadFloat3(pDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints_InWorld[POINT_B]), XMLoadFloat3(pDestPoint)))
			return true;
	}
	return false;
}

_bool CCell::isOut(_fvector vWorldPosition, _fmatrix WorldMatrix, _int* pNeighborIndex, _vector vLook, __out _vector* pSliding)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		_vector		vSour = XMVector3Normalize(XMVectorSetW(vWorldPosition - XMLoadFloat3(&m_vPoints_InWorld[i]), 0.f));
		_vector		vDest = XMVector3Normalize(XMLoadFloat3(&m_vNormals[i]));

		_float fRadian = XMVectorGetX(XMVector3Dot(vSour, vDest));
		if (0.f <= fRadian)
		{
			*pNeighborIndex = m_iNeighborIndices[i];

			if (nullptr != pSliding)
			{
				_vector vSliding = XMVector3Normalize(XMVectorSet(m_vNormals[i].z, 0.f, m_vNormals[i].x * -1.f, 0.f));	
				if(XMVectorGetX(XMVector3Dot(vSliding, vLook)) >= XMConvertToRadians(0.f) && XMVectorGetX(XMVector3Dot(vSliding, vLook)) <= XMConvertToRadians(90.f))
					*pSliding = vSliding;
				else
					*pSliding = -1.f * vSliding;
			}
				

			return true;
		}
	}

	return false;
}

#ifdef _DEBUG

_bool CCell::Is_InCell(_vector vWorldPosition)
{

	_float fDistance = 0.f;
	XMVectorSetW(vWorldPosition, 1.f);
	XMVectorSetY(vWorldPosition, XMVectorGetY(vWorldPosition) + 0.1f);
	if (TriangleTests::Intersects(XMVectorSetY(vWorldPosition, 1.f), XMVectorSet(0.f, -1.f, 0.f, 0.f)
		, XMVectorSetY(XMLoadFloat3(&m_vPoints_InWorld[POINTS::POINT_A]), 0.f)
		, XMVectorSetY(XMLoadFloat3(&m_vPoints_InWorld[POINTS::POINT_B]), 0.f)
		, XMVectorSetY(XMLoadFloat3(&m_vPoints_InWorld[POINTS::POINT_C]), 0.f)
		, fDistance))
		return true;



	return false;
}

_bool CCell::Is_Equal(CCell* pOtherCell)
{
	_bool bCompares[3] = { false, false, false };
	for (_uint i = 0; i < POINT_END; ++i)
	{
		for (_uint j = 0; j < POINT_END; ++j)
		{
			if ((m_vPoints_InWorld[i].x == pOtherCell->m_vPoints_InWorld[j].x)
				&& m_vPoints_InWorld[i].z == pOtherCell->m_vPoints_InWorld[j].z)
			{
				bCompares[i] = true;
			}
		}	
	}
	
	return ((bCompares[0] == true)
		&& (bCompares[1] == true) 
		&& (bCompares[2] == true));
}

HRESULT CCell::Render(CShader* pShader)
{


	_bool bNeighbor = false;
	bNeighbor |= m_iNeighborIndices[0] == -1;
	bNeighbor |= m_iNeighborIndices[1] == -1; 
	bNeighbor |= m_iNeighborIndices[2] == -1;

	if (!bNeighbor)
	{
		_float4 vLineColor = _float4(1.f, 0.f, 0.f, 1.f);
		if (FAILED(pShader->Bind_RawValue("g_vLineColor", &vLineColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_vLineColor", &vLineColor, sizeof(_float4))))
			return E_FAIL;

		_float		fHeight = 0.05f;
		if (FAILED(pShader->Bind_RawValue("g_fHeight", &fHeight, sizeof(_float))))
			return E_FAIL;
	}
	else
	{
		_float4 vLineColor = _float4(0.f, 1.f, 0.f, 1.f);
		if (FAILED(pShader->Bind_RawValue("g_vLineColor", &vLineColor, sizeof(_float4))))
			return E_FAIL;

		_float		fHeight = 0.02f;
		if (FAILED(pShader->Bind_RawValue("g_fHeight", &fHeight, sizeof(_float))))
			return E_FAIL;
	}

	pShader->Begin(0);
	m_pVIBuffer->Render();
	return S_OK;
}


#endif

CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3 * pPoints, _uint iIndex)
{
	CCell*	pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Created : CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CELL_DESC& tDesc, vector<_float3>& Points)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(tDesc, Points)))
	{
		MSG_BOX("Failed to Created : CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

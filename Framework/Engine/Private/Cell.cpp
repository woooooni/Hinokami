#include "..\Public\Cell.h"
#include "VIBuffer_Cell.h"

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

	memcpy(m_vPoints_Original, pPoints, sizeof(_float3) * POINT_END);

	_float3		vLines[LINE_END];

	XMStoreFloat3(&vLines[LINE_AB], XMLoadFloat3(&m_vPoints_Original[POINT_B]) - XMLoadFloat3(&m_vPoints_Original[POINT_A]));
	XMStoreFloat3(&vLines[LINE_BC], XMLoadFloat3(&m_vPoints_Original[POINT_C]) - XMLoadFloat3(&m_vPoints_Original[POINT_B]));
	XMStoreFloat3(&vLines[LINE_CA], XMLoadFloat3(&m_vPoints_Original[POINT_A]) - XMLoadFloat3(&m_vPoints_Original[POINT_C]));

	for (size_t i = 0; i < LINE_END; i++)	
	{
		m_vNormals[i] = _float3(vLines[i].z * -1.f, 0.f, vLines[i].x);

		XMStoreFloat3(&m_vNormals[i], XMVector3Normalize(XMLoadFloat3(&m_vNormals[i])));
	}

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints_Original);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

void CCell::Update(_fmatrix WorldMatrix)
{
	for (size_t i = 0; i < POINT_END; i++)
	{
		XMStoreFloat3(&m_vPoints_InWorld[i],
			XMVector3TransformCoord(XMLoadFloat3(&m_vPoints_Original[i]), WorldMatrix));
	}

}

_bool CCell::Compare_Points(const _float3 * pSourPoint, const _float3 * pDestPoint)
{
	
	//_vector		vResult = XMVectorEqual(A, B);
	//XMVector3Equal( A, B);

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints_Original[POINT_A]), XMLoadFloat3(pSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints_Original[POINT_B]), XMLoadFloat3(pDestPoint)))
			return true;		

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints_Original[POINT_C]), XMLoadFloat3(pDestPoint)))
			return true;		
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints_Original[POINT_B]), XMLoadFloat3(pSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints_Original[POINT_A]), XMLoadFloat3(pDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints_Original[POINT_C]), XMLoadFloat3(pDestPoint)))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints_Original[POINT_C]), XMLoadFloat3(pSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints_Original[POINT_A]), XMLoadFloat3(pDestPoint)))
			return true;

		if (XMVector3Equal(XMLoadFloat3(&m_vPoints_Original[POINT_B]), XMLoadFloat3(pDestPoint)))
			return true;
	}
	return false;
}

_bool CCell::isOut(_fvector vPoint, _fmatrix WorldMatrix, _int* pNeighborIndex)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		_vector		vSour = XMVector3Normalize(vPoint - XMLoadFloat3(&m_vPoints_InWorld[i]));
		_vector		vDest = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&m_vNormals[i]), WorldMatrix));

		if (0 < XMVectorGetX(XMVector3Dot(vSour, vDest)))
		{
			*pNeighborIndex = m_iNeighborIndices[i];
			return true;
		}
	}

	return false;
}

HRESULT CCell::Create_Cell(const _float3* pPoints)
{
	for (_uint i = 0; i < POINT_END; ++i)
	{
		
	}
	return S_OK;
}

#ifdef _DEBUG

HRESULT CCell::Render()
{
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

void CCell::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

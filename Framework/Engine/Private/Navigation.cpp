#include "..\Public\Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "PipeLine.h"

_float4x4 CNavigation::m_WorldMatrix = {};

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_iCurrentIndex(rhs.m_iCurrentIndex)
	, m_Cells(rhs.m_Cells)	
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif
{

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif

	for (auto& pCell : m_Cells)	
		Safe_AddRef(pCell);
}

HRESULT CNavigation::Initialize_Prototype(const wstring & strNavigationDataFiles)
{
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(strNavigationDataFiles.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		_float3		vPoints[CCell::POINT_END] = {};

		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CCell*		pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;	

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS_DECLARATION::Elements, VTXPOS_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{	
	if (nullptr == pArg)
		return S_OK;

	NAVIGATION_DESC*		pNaviDesc = (NAVIGATION_DESC*)pArg;

	/*  이 네비게이션을 이용하고자하는 객체가 어떤 셀에 있는지 저장한다. */
	m_iCurrentIndex = pNaviDesc->iCurrentIndex;

	return S_OK;
}

void CNavigation::Update(_fmatrix WorldMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);

	for (auto& pCell : m_Cells)
	{
		if (nullptr != pCell)
			pCell->Update(WorldMatrix);
	}
}

_bool CNavigation::isMove(_fvector vPoint)
{
	_int		iNeighborIndex = 0;

	if (true == m_Cells[m_iCurrentIndex]->isOut(vPoint, XMLoadFloat4x4(&m_WorldMatrix), &iNeighborIndex))
	{
		/* 나간 방향에 이웃셀이 있으면 움직여야해! */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (false == m_Cells[iNeighborIndex]->isOut(vPoint, XMLoadFloat4x4(&m_WorldMatrix), &iNeighborIndex))
				{
					m_iCurrentIndex = iNeighborIndex;
					break;
				}
			}			
			return true;
		}
		else
			return false;

	}
	else
		return true;	
}

_float CNavigation::Compute_Height(_vector vPosition)
{
	CCell* pCell = m_Cells[m_iCurrentIndex];

	_vector vA = XMVectorSetW(XMLoadFloat3(pCell->Get_Point(CCell::POINT_A)), 1.f);
	_vector vB = XMVectorSetW(XMLoadFloat3(pCell->Get_Point(CCell::POINT_B)), 1.f);
	_vector vC = XMVectorSetW(XMLoadFloat3(pCell->Get_Point(CCell::POINT_C)), 1.f);

	_vector vPlane = XMPlaneFromPoints(vA, vB, vC);


	_vector fTargetPos = vPosition;
	_float fx = XMVectorGetX(fTargetPos);
	_float fz = XMVectorGetZ(fTargetPos);

	_float fa = XMVectorGetX(vPlane);
	_float fb = XMVectorGetY(vPlane);
	_float fc = XMVectorGetZ(vPlane);
	_float fd = XMVectorGetW(vPlane);

	/* y = (-ax - cz - d) / b */

	return (-fa * fx - fc * fz - fd) / fb;
}

HRESULT CNavigation::Create_Cell(const _float3* vLocalPoints)
{
	vector<_float3> LocalPointsCW;
	for (_uint i = 0; i < CCell::POINTS::POINT_END; ++i)
	{
		LocalPointsCW.push_back(vLocalPoints[i]);
	}

	if (0 < Compute_CW(LocalPointsCW[0], LocalPointsCW[1], LocalPointsCW[2]))
		swap(LocalPointsCW[1], LocalPointsCW[2]);


	CCell* pNewCell = CCell::Create(m_pDevice, m_pContext, LocalPointsCW.data(), m_Cells.size());
	if (nullptr == pNewCell)
		return E_FAIL;


	m_Cells.push_back(pNewCell);
	return S_OK;
}


#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);



	if (-1 == m_iCurrentIndex)
	{
		_float4 vLineColor = _float4(0.f, 1.f, 0.f, 1.f);
		if (FAILED(m_pShader->Bind_RawValue("g_vLineColor", &vLineColor, sizeof(_float4))))
			return E_FAIL;

		_float		fHeight = 0.f;
		if (FAILED(m_pShader->Bind_RawValue("g_fHeight", &fHeight, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(0)))
			return E_FAIL;

		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
				pCell->Render();
		}
	}
	else
	{
		_float4 vLineColor = _float4(1.f, 0.f, 0.f, 1.f);
		if (FAILED(m_pShader->Bind_RawValue("g_vLineColor", &vLineColor, sizeof(_float4))))
			return E_FAIL;

		_float		fHeight = 0.1f;
		if (FAILED(m_pShader->Bind_RawValue("g_fHeight", &fHeight, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(0)))
			return E_FAIL;

		m_Cells[m_iCurrentIndex]->Render();
	}	

	return S_OK;
}

#endif

int CNavigation::Compute_CW(_float3 vPointA, _float3 vPointB, _float3 vPointC)
{
	return (vPointB.x - vPointA.x) * (vPointC.z - vPointA.z) - (vPointB.z - vPointA.z) * (vPointC.x - vPointA.x);
}

/* 네비게이션을 구성하는 각각의 셀들의 이웃을 설정한다. */
HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDestCell);
			}

			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDestCell);
			}

			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDestCell);
			}
		}
	}
	
	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strNavigationDataFiles)
{
	CNavigation*	pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strNavigationDataFiles)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*	pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
}

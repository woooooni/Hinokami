#include "..\Public\Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "PipeLine.h"
#include "FileUtils.h"
#include <filesystem>
#include "GameInstance.h"
#include <set>

_float4x4 CNavigation::m_WorldIdentity = {};

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_iCurrentIndex(-1)
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
	

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS_DECLARATION::Elements, VTXPOS_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	XMStoreFloat4x4(&m_WorldIdentity, XMMatrixIdentity());

	if (FAILED(Load_NaviData(strNavigationDataFiles)))
		return S_OK;

	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{	
	if (nullptr == pArg)
		return S_OK;

	NAVIGATION_DESC*		pNaviDesc = (NAVIGATION_DESC*)pArg;
	m_NavigationDesc = *pNaviDesc;

	/*  이 네비게이션을 이용하고자하는 객체가 어떤 셀에 있는지 저장한다. */
	if (pNaviDesc->bInitialize_Index)
	{
		if (FAILED(Initialize_Index(XMLoadFloat4(&pNaviDesc->vStartWorldPosition))))
		{
			
			_float3 vPointA = *m_Cells[0]->Get_PointWorld(CCell::POINTS::POINT_A);
			_float3 vPointB = *m_Cells[0]->Get_PointWorld(CCell::POINTS::POINT_B);
			_float3 vPointC = *m_Cells[0]->Get_PointWorld(CCell::POINTS::POINT_C);

			_float4 vPosition; 
			XMStoreFloat4(&vPosition, 
				(XMLoadFloat3(&vPointA)
				+ XMLoadFloat3(&vPointB)
				+ XMLoadFloat3(&vPointC)) / 3.f);
			vPosition.w = 1.f;

			m_NavigationDesc.vStartWorldPosition = vPosition;
			m_iCurrentIndex = m_Cells[0]->Get_Index();
			return S_OK;
		}
		
	}
		
	else
	{
		m_iCurrentIndex = -1;
	}
		
	

	return S_OK;
}

void CNavigation::Update(_fmatrix WorldMatrix)
{
	for (auto& pCell : m_Cells)
	{
		if (nullptr != pCell)
			pCell->Update(WorldMatrix);
	}
}

_bool CNavigation::Is_Movable(_fvector vPoint, _vector vLook, __out _vector* vOutSlidingDir)
{
	_int		iNeighborIndex = 0;
	if (true == m_Cells[m_iCurrentIndex]->isOut(vPoint, XMLoadFloat4x4(&m_WorldIdentity), &iNeighborIndex, vLook, vOutSlidingDir))
	{
		/* 나간 방향에 이웃셀이 있으면 움직여야해! */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (false == m_Cells[iNeighborIndex]->isOut(vPoint, XMLoadFloat4x4(&m_WorldIdentity), &iNeighborIndex, vLook, vOutSlidingDir))
				{
					m_iCurrentIndex = iNeighborIndex;
					break;
				}
			}
			return true;
		}
		else
		{
			return false;
		}

	}
	else
		return true;	
}

_float CNavigation::Compute_Height(_vector vPosition)
{
	CCell* pCell = m_Cells[m_iCurrentIndex];

	_vector vA = XMVectorSetW(XMLoadFloat3(pCell->Get_PointWorld(CCell::POINT_A)), 1.f);
	_vector vB = XMVectorSetW(XMLoadFloat3(pCell->Get_PointWorld(CCell::POINT_B)), 1.f);
	_vector vC = XMVectorSetW(XMLoadFloat3(pCell->Get_PointWorld(CCell::POINT_C)), 1.f);

	_vector vPlane = XMPlaneFromPoints(vA, vB, vC);


	_vector fTargetPos = vPosition;
	_float fx = XMVectorGetX(fTargetPos);
	_float fz = XMVectorGetZ(fTargetPos);

	_float fa = XMVectorGetX(vPlane);
	_float fb = XMVectorGetY(vPlane);
	_float fc = XMVectorGetZ(vPlane);
	_float fd = XMVectorGetW(vPlane);

	_float fResult = (-fa * fx - fc * fz - fd) / fb;
	return fResult;
}

HRESULT CNavigation::Create_Cell(const _float3* vLocalPoints)
{
	vector<_float3> LocalPointsCW;
	for (_uint i = 0; i < CCell::POINTS::POINT_END; ++i)
		LocalPointsCW.push_back(vLocalPoints[i]);

	if (0.f > Compute_CW(LocalPointsCW[0], LocalPointsCW[1], LocalPointsCW[2]))
		swap(LocalPointsCW[1], LocalPointsCW[2]);


	CCell* pNewCell = CCell::Create(m_pDevice, m_pContext, LocalPointsCW.data(), m_Cells.size());
	if (nullptr == pNewCell)
		return E_FAIL;


	m_Cells.push_back(pNewCell);
	return S_OK;
}

HRESULT CNavigation::Delete_Cell(const _fvector vMouseWorldDir, const _fvector vMouseWorldPosition)
{
	auto iter = m_Cells.begin();
	while (iter != m_Cells.end())
	{
		_float3 vPointA = *(*iter)->Get_PointWorld(CCell::POINT_A);
		_float3 vPointB = *(*iter)->Get_PointWorld(CCell::POINT_B);
		_float3 vPointC = *(*iter)->Get_PointWorld(CCell::POINT_C);
		_float fDistance = 9999999.f;

		if (TriangleTests::Intersects(vMouseWorldPosition, XMVector3Normalize(vMouseWorldDir),
			XMLoadFloat3(&vPointA),
			XMLoadFloat3(&vPointB),
			XMLoadFloat3(&vPointC), fDistance))
		{
			iter = m_Cells.erase(iter);
			return S_OK;
		}

		++iter;
	}

	return S_OK;
}
HRESULT CNavigation::Clear_Cells()
{
	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();
	return S_OK;
}

HRESULT CNavigation::Save_NaviData(const wstring& strNaviDataPath)
{
	auto path = filesystem::path(strNaviDataPath);
	filesystem::create_directories(path.parent_path());


	shared_ptr<CFileUtils> File = make_shared<CFileUtils>();
	File->Open(strNaviDataPath, FileMode::Write);


	File->Write<_int>(m_Cells.size());
	for (auto& pCell : m_Cells)
	{
		File->Write<_uint>(pCell->m_iIndex);
		File->Write(pCell->m_bMovable);

		for (_uint i = 0; i < CCell::POINT_END; ++i)		
			File->Write<_float3>(pCell->m_vPoints_InWorld[i]);

		for (_uint i = 0; i < CCell::LINE_END; ++i)
			File->Write<_int>(pCell->m_iNeighborIndices[i]);

		
	}
	return S_OK;
}

HRESULT CNavigation::Load_NaviData(const wstring& strNaviDataPath)
{
	Clear_Cells();

	if (!filesystem::exists(strNaviDataPath))
		return E_FAIL;

	shared_ptr<CFileUtils> File = make_shared<CFileUtils>();
	File->Open(strNaviDataPath, FileMode::Read);

	_int iNumCells = 0; 
	File->Read<_int>(iNumCells);

	vector<_float3> Points;
	Points.reserve(3);
	for (_int i = 0; i < iNumCells; ++i)
	{
		CCell::CELL_DESC tDesc;
		ZeroMemory(&tDesc, sizeof(CCell::CELL_DESC));
		tDesc.iIndex = File->Read<_uint>();
		tDesc.bMovable = File->Read<_bool>();

		for (_uint i = 0; i < CCell::POINT_END; ++i)
			Points.push_back(File->Read<_float3>());

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, tDesc, Points);

		if (nullptr == pCell)
			return E_FAIL;

		for (_uint i = 0; i < CCell::LINE_END; ++i)
			File->Read<_int>(pCell->m_iNeighborIndices[i]);

		Points.clear();
		m_Cells.push_back(pCell);
	}



	return S_OK;
}


#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldIdentity)))
		return E_FAIL;

	_float4x4 ViewMatirix = GI->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &ViewMatirix)))
		return E_FAIL;

	_float4x4 ProjMatirx = GI->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &ProjMatirx)))
		return E_FAIL;



	if (-1 == m_iCurrentIndex)
	{
		_float4 vLineColor = _float4(0.f, 1.f, 0.f, 1.f);
		if (FAILED(m_pShader->Bind_RawValue("g_vLineColor", &vLineColor, sizeof(_float4))))
			return E_FAIL;

		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
				pCell->Render(m_pShader);
		}
	}
	else
	{
		_float4 vLineColor = _float4(1.f, 0.f, 0.f, 1.f);
		if (FAILED(m_pShader->Bind_RawValue("g_vLineColor", &vLineColor, sizeof(_float4))))
			return E_FAIL;

		_float		fHeight = 0.02f;
		if (FAILED(m_pShader->Bind_RawValue("g_fHeight", &fHeight, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(0)))
			return E_FAIL;

		m_Cells[m_iCurrentIndex]->Render(m_pShader);
	}	

	return S_OK;
}

#endif

_float CNavigation::Compute_CW(_float3 vPointA, _float3 vPointB, _float3 vPointC)
{
	_vector vLineAB = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&vPointB) - XMLoadFloat3(&vPointA), 0.f));
	_vector vLineAC = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&vPointC) - XMLoadFloat3(&vPointA), 0.f));

	_float vNormalY = XMVectorGetY(XMVector3Cross(vLineAB, vLineAC));
	return vNormalY;
}

HRESULT CNavigation::Initialize_Index(_vector vWorldPostion)
{
	m_iCurrentIndex = -1;

	for (auto& pCell : m_Cells)
	{
		if (true == pCell->Is_InCell(vWorldPostion))
			m_iCurrentIndex = pCell->Get_Index();
	}

	if (m_iCurrentIndex == -1)
		return E_FAIL;

	return S_OK;
}

/* 네비게이션을 구성하는 각각의 셀들의 이웃을 설정한다. */
HRESULT CNavigation::SetUp_Neighbors()
{
	

	_uint iCurrentIndex = 0;
	
	for (auto& pCell : m_Cells)
	{
		pCell->Set_Index(iCurrentIndex++);
		pCell->Reset_Neighbor();
	}

	Delete_Duplicate_Cell();

	iCurrentIndex = 0;
	for (auto& pCell : m_Cells)
	{
		pCell->Set_Index(iCurrentIndex++);
		pCell->Reset_Neighbor();
	}

	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_PointWorld(CCell::POINT_A), pSourCell->Get_PointWorld(CCell::POINT_B)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDestCell);
			}

			else if (true == pDestCell->Compare_Points(pSourCell->Get_PointWorld(CCell::POINT_B), pSourCell->Get_PointWorld(CCell::POINT_C)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDestCell);
			}

			else if (true == pDestCell->Compare_Points(pSourCell->Get_PointWorld(CCell::POINT_C), pSourCell->Get_PointWorld(CCell::POINT_A)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDestCell);
			}
		}
	}
	return S_OK;
}


HRESULT CNavigation::Delete_Duplicate_Cell()
{
	auto iterSour = m_Cells.begin();

	while (iterSour != m_Cells.end())
	{
		auto iterDest = m_Cells.begin();
		while (iterDest != m_Cells.end())
		{
			if ((*iterSour)->Get_Index() == (*iterDest)->Get_Index())
			{
				iterDest++;
				continue;
			}
				

			if ((*iterSour)->Is_Equal(*iterDest))
			{
				iterDest = m_Cells.erase(iterDest);
				continue;
			}	
			iterDest++;
		}
		iterSour++;
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



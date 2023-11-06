#include "stdafx.h"
#include "Picking_Manager.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Transform.h"
#include "Cell.h"

IMPLEMENT_SINGLETON(CPicking_Manager)

CPicking_Manager::CPicking_Manager()
{

}

HRESULT CPicking_Manager::Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	

	return S_OK;
}

void CPicking_Manager::Tick(_float fTimeDelta)
{

}

_bool CPicking_Manager::Is_Picking(CTransform* pTransform, CVIBuffer* pBuffer, _bool bCutPos, _float4* vOut)
{
	if (nullptr == pTransform)
		return false;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	_vector vMousePos = XMVectorSet(
		_float(pt.x / (g_iWinSizeX * .5f) - 1.f),
		_float(pt.y / (g_iWinSizeY * -.5f) + 1.f),
		1.f, 1.f);



	_matrix ViewMatrixInv = GAME_INSTANCE->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);
	_matrix ProjMatrixInv = GAME_INSTANCE->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_PROJ);

	vMousePos = XMVector3TransformCoord(vMousePos, ProjMatrixInv);

	XMVECTOR vRayDir, vRayPosition;

	vRayPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	vRayDir = vMousePos - vRayPosition;


	vMousePos = XMVector3TransformCoord(vRayPosition, ViewMatrixInv);
	vRayDir = XMVector3TransformNormal(vRayDir, ViewMatrixInv);


	_matrix WordlMatrixInv = pTransform->Get_WorldMatrixInverse();

	vRayPosition = XMVector3TransformCoord(vMousePos, WordlMatrixInv);
	vRayDir = XMVector3TransformNormal(vRayDir, WordlMatrixInv);

	vRayDir = XMVector3Normalize(vRayDir);

	_float fDistnace = 0.f;
	_float fMinDistance = 999999.f;

	const vector<_float3>& Vertices = pBuffer->Get_VertexLocalPositions();
	const vector<FACEINDICES32>& Indices = pBuffer->Get_FaceIndices();

	for (_uint i = 0; i < Indices.size(); ++i)
	{
		_vector v0, v1, v2;
		v0 = XMVectorSet(Vertices[Indices[i]._0].x, Vertices[Indices[i]._0].y, Vertices[Indices[i]._0].z, 1.f);
		v1 = XMVectorSet(Vertices[Indices[i]._1].x, Vertices[Indices[i]._1].y, Vertices[Indices[i]._1].z, 1.f);
		v2 = XMVectorSet(Vertices[Indices[i]._2].x, Vertices[Indices[i]._2].y, Vertices[Indices[i]._2].z, 1.f);

		if (true == TriangleTests::Intersects(vRayPosition, vRayDir, v0, v1, v2, fDistnace))
		{
			if (fDistnace < fMinDistance)
			{
				_float4 vPickingPos;
				XMStoreFloat4(&vPickingPos, 
					XMVector3TransformCoord(vRayPosition, pTransform->Get_WorldMatrix()) 
					+ XMVector3TransformNormal(vRayDir, pTransform->Get_WorldMatrix()) * fDistnace);

				if (vOut != nullptr)
				{
					if (bCutPos)
					{
						vPickingPos.x = floorf(vPickingPos.x + 0.5f);
						vPickingPos.y = floorf(vPickingPos.y + 0.5f);
						vPickingPos.z = floorf(vPickingPos.z + 0.5f);
					}
					*vOut = vPickingPos;
				}
				fMinDistance = fDistnace;
			}
		}
	}
	return fMinDistance < 999999.f;
}

_bool CPicking_Manager::Is_NaviPicking(CTransform* pTransform, CVIBuffer* pBuffer, _float3* pWorldOut, _float3* pLocalPos)
{
	if (nullptr == pTransform)
		return false;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	_vector vMousePos = XMVectorSet(
		_float(pt.x / (g_iWinSizeX * .5f) - 1.f),
		_float(pt.y / (g_iWinSizeY * -.5f) + 1.f),
		1.f, 1.f);



	_matrix ViewMatrixInv = GAME_INSTANCE->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);
	_matrix ProjMatrixInv = GAME_INSTANCE->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_PROJ);

	vMousePos = XMVector3TransformCoord(vMousePos, ProjMatrixInv);

	XMVECTOR vRayDir, vRayPosition;

	vRayPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	vRayDir = vMousePos - vRayPosition;


	vMousePos = XMVector3TransformCoord(vRayPosition, ViewMatrixInv);
	vRayDir = XMVector3TransformNormal(vRayDir, ViewMatrixInv);


	_matrix WordlMatrixInv = pTransform->Get_WorldMatrixInverse();

	vRayPosition = XMVector3TransformCoord(vMousePos, WordlMatrixInv);
	vRayDir = XMVector3TransformNormal(vRayDir, WordlMatrixInv);

	vRayDir = XMVector3Normalize(vRayDir);

	_float fDistnace = 0.f;
	_float fMinDistance = 999999.f;

	const vector<_float3>& Vertices = pBuffer->Get_VertexLocalPositions();
	const vector<FACEINDICES32>& Indices = pBuffer->Get_FaceIndices();

	for (_uint i = 0; i < Indices.size(); ++i)
	{
		_vector vA, vB, vC;
		vA = XMVectorSet(Vertices[Indices[i]._0].x, Vertices[Indices[i]._0].y, Vertices[Indices[i]._0].z, 1.f);
		vB = XMVectorSet(Vertices[Indices[i]._1].x, Vertices[Indices[i]._1].y, Vertices[Indices[i]._1].z, 1.f);
		vC = XMVectorSet(Vertices[Indices[i]._2].x, Vertices[Indices[i]._2].y, Vertices[Indices[i]._2].z, 1.f);

		if (true == TriangleTests::Intersects(vRayPosition, vRayDir, vA, vB, vC, fDistnace))
		{
			if (fDistnace < fMinDistance)
			{
				_vector vPickingPos =
					XMVector3TransformCoord(vRayPosition, pTransform->Get_WorldMatrix())
					+ XMVector3TransformNormal(vRayDir, pTransform->Get_WorldMatrix()) * fDistnace;

				_float fAPointDist = XMVectorGetX(XMVector3Length(vA - vPickingPos));
				_float fBPointDist = XMVectorGetX(XMVector3Length(vB - vPickingPos));
				_float fCPointDist = XMVectorGetX(XMVector3Length(vC - vPickingPos));

				if (pLocalPos != nullptr)
				{
					_vector vNearBufferPosition;
					if (fAPointDist >= fBPointDist)
					{
						// A > B
						vNearBufferPosition = vA;

						// B > C
						if (fCPointDist <= fBPointDist)
							vNearBufferPosition = vB;
					}
					else
					{
						// A < B
						vNearBufferPosition = vC;

						// C < A
						if (fCPointDist <= fAPointDist)
							vNearBufferPosition = vB;
					}

					XMStoreFloat3(pLocalPos, vNearBufferPosition);

					if (pWorldOut != nullptr)
						XMStoreFloat3(pWorldOut, XMVector3TransformCoord(vNearBufferPosition, pTransform->Get_WorldMatrix()));
				}
				fMinDistance = fDistnace;
			}
		}
	}
	return fMinDistance < 999999.f;
}

_bool CPicking_Manager::Is_JustNaviPicking(CNavigation* pNavigation, _float3* pWorldOut)
{

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	_vector vMousePos = XMVectorSet(
		_float(pt.x / (g_iWinSizeX * .5f) - 1.f),
		_float(pt.y / (g_iWinSizeY * -.5f) + 1.f),
		1.f, 1.f);

	_matrix ViewMatrixInv = GAME_INSTANCE->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);
	_matrix ProjMatrixInv = GAME_INSTANCE->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_PROJ);

	vMousePos = XMVector3TransformCoord(vMousePos, ProjMatrixInv);

	XMVECTOR vRayDir, vRayPosition;

	vRayPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	vRayDir = vMousePos - vRayPosition;


	vMousePos = XMVector3TransformCoord(vRayPosition, ViewMatrixInv);
	vRayDir = XMVector3TransformNormal(vRayDir, ViewMatrixInv);

	const vector<CCell*>& Cells = pNavigation->Get_Cells();
	auto iter = Cells.begin();
	while (iter != Cells.end())
	{
		_float3 vPointA = *(*iter)->Get_PointWorld(CCell::POINT_A);
		_float3 vPointB = *(*iter)->Get_PointWorld(CCell::POINT_B);
		_float3 vPointC = *(*iter)->Get_PointWorld(CCell::POINT_C);


		_float fDistance = 9999999.f;
		_float fNearDistance = 0.f;
		if (TriangleTests::Intersects(vMousePos, XMVector3Normalize(vRayDir),
			XMLoadFloat3(&vPointA),
			XMLoadFloat3(&vPointB),
			XMLoadFloat3(&vPointC), fDistance))
		{
			_vector vPickingPos =
				vRayPosition + vRayDir * fDistance;

			
			_float fAPointDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPointA) - vPickingPos));
			_float fBPointDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPointB) - vPickingPos));
			_float fCPointDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPointC) - vPickingPos));

			fNearDistance = min(fAPointDist, fBPointDist);
			fNearDistance = min(fNearDistance, fCPointDist);

			if (fNearDistance == fAPointDist)
			{
				*pWorldOut = vPointA;
			}
			else if (fNearDistance == fBPointDist)
			{
				*pWorldOut = vPointB;
			}
			else if (fNearDistance == fCPointDist)
			{
				*pWorldOut = vPointC;
			}
			return true;
		}

		++iter;
	}

	return false;
}




void CPicking_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

#include "stdafx.h"
#include "Picking_Manager.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Transform.h"

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

template<typename T>
_bool CPicking_Manager::Is_Picking(CTransform* pTransform, const  vector<T>& Vertices, const vector<FACEINDICES32>& Indices, _uint iPrimitiveCount, __out _vector* vOut)
{
	if (nullptr == pTransform)
		return false;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	_vector vMousePos = XMVectorSet(
		_float(pt.x / (g_iWinSizeX * .5f) - 1.f),
		_float(pt.y / (g_iWinSizeY * -.5f) + 1.f),
		0.f, 0.f);


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



	for (_uint i = 0; i < iPrimitiveCount; ++i)
	{
		_vector v0, v1, v2;
		v0 = XMVectorSet(Vertices[Indices[i]._0].vPosition.x, Vertices[Indices[i]._0].vPosition.y, Vertices[Indices[i]._0].vPosition.z, 1.f);
		v1 = XMVectorSet(Vertices[Indices[i]._1].vPosition.x, Vertices[Indices[i]._1].vPosition.y, Vertices[Indices[i]._1].vPosition.z, 1.f);
		v2 = XMVectorSet(Vertices[Indices[i]._2].vPosition.x, Vertices[Indices[i]._2].vPosition.y, Vertices[Indices[i]._2].vPosition.z, 1.f);



		if (true == TriangleTests::Intersects(vRayPosition, vRayDir, v0, v1, v2, fDistnace))
		{
			if (fDistnace < fMinDistance)
			{
				_float4 vPickingPos;
				XMStoreFloat4(&vPickingPos, vRayPosition + vRayDir * fDistnace);
				
				if (vOut != nullptr)
				{
					vPickingPos.x = floorf(vPickingPos.x + 0.5f);
					vPickingPos.y = floorf(vPickingPos.y + 0.5f);
					vPickingPos.z = floorf(vPickingPos.z + 0.5f);
					*vOut = XMLoadFloat4(&vPickingPos);
				}
				
				return true;
			}
		}
	}

	return false;
}

void CPicking_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

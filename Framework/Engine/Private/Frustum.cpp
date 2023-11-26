#include "..\Public\Frustum.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{
	
}

HRESULT CFrustum::Initialize()
{
	m_vOriginalPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vOriginalPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vOriginalPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vOriginalPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vOriginalPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vOriginalPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vOriginalPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vOriginalPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Tick()
{
	
	_matrix		ProjMatrixInv = GI->Get_TransformMatrixInverse(CPipeLine::D3DTS_PROJ);
	_matrix		ViewMatrixInv = GI->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW);

	/* 여덟개 점을 월드까지 변환시키자. */
	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat3(&m_vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vOriginalPoints[i]), ProjMatrixInv));
		XMStoreFloat3(&m_vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), ViewMatrixInv));
	}

	/* 월드 상의 평면 여섯개를 구성하자. */
	Make_Planes(m_vPoints, m_WorldPlanes);
}

_bool CFrustum::Intersect_Frustum_World(_fvector vWorldPos, _float fRadius)
{
	/* 0 < ax + by + cz + d */

	/* Plane		: a b c d */
	/* vWorldPos	: x y z 1 */
	fRadius = XMConvertToRadians(fRadius);
	for (size_t i = 0; i < 6; i++)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_WorldPlanes[i]), vWorldPos)))
			return false;		
	}

	return true;
}

void CFrustum::Make_Planes(const _float3 * pPoints, _float4 * pPlanes)
{
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[6])));
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[3])));
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[1])));
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat3(&pPoints[3]), XMLoadFloat3(&pPoints[2]), XMLoadFloat3(&pPoints[6])));
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[7])));
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[2])));
}

void CFrustum::Free()
{
	
}

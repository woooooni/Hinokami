#pragma once

/* 절두체를 의미하는 객체. */
/* 절두체 == 여섯개면을 의미. */
/* 최초엔 투영스페이스 상의 평면을 구성할 수 있는 여덟개 점의 정보를 가진다. */
/* 매 프레임마다 이 여덟개 점을 월드 스페이스로 변환시켜놓는다. */

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Tick();
	_bool Intersect_Frustum_World(_fvector vWorldPos, _float fRadius);

private:
	_float3			m_vOriginalPoints[8];
	_float3			m_vPoints[8];

	_float4			m_WorldPlanes[6];

private:
	void Make_Planes(const _float3* pPoints, _float4* pPlanes);

public:
	virtual void Free() override;
};

END
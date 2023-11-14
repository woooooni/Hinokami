#pragma once

/* ����ü�� �ǹ��ϴ� ��ü. */
/* ����ü == ���������� �ǹ�. */
/* ���ʿ� ���������̽� ���� ����� ������ �� �ִ� ������ ���� ������ ������. */
/* �� �����Ӹ��� �� ������ ���� ���� �����̽��� ��ȯ���ѳ��´�. */

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
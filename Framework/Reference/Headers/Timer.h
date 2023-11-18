#pragma once
#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CTimer :	public CBase
{
	
private:
	CTimer();
	virtual ~CTimer() = default;
	
public:
	HRESULT	Ready_Timer();
	_float	Compute_TimeDelta();

public:
	void Set_TimeScale(_float fTimeScale) { m_fTimeScale = fTimeScale; }
	_float Get_TimeScale() { return m_fTimeScale; }

	_float Get_TimeDelta() { return m_fTimeDelta; }
	
private:
	LARGE_INTEGER			m_FrameTime;
	LARGE_INTEGER			m_FixTime;
	LARGE_INTEGER			m_LastTime;
	LARGE_INTEGER			m_CpuTick;

	_float					m_fTimeDelta;

	_float					m_fTimeScale = 1.f;

public:
	static CTimer*		Create(void);
private:
	virtual void Free();
};

END
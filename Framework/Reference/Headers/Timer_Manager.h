#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimer_Manager : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)

private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	/* 타임델타를 얻어온다. */
	/* 타임델타를 계산한다. */
	_float Compute_TimeDelta(const wstring& strTimerTag);


public:
	HRESULT Set_TimeScale(const wstring& strTimerTag, _float fTimeScale);
	_float Get_TimeScale(const wstring& strTimerTag);

	_float Get_TimeDelta(const wstring& strTimerTag);
	HRESULT Set_Slow(const wstring& strTimerTag, _float fSlowTime, _float fTimeScale, _bool bForce);

public:
	HRESULT			Add_Timer(const wstring& strTimerTag);
	class CTimer* Find_Timer(const wstring& strTimerTag);



private:
	map<const wstring, class CTimer*>		m_Timers;



public:
	virtual void	Free(void);
};

END
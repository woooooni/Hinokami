#pragma once

#include "Timer.h"

BEGIN(Engine)

class CTimer_Manager final : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager);
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	_float Get_TimeDelta(const wstring& pTimerTag);

public:
	HRESULT Add_Timer(const wstring& pTimerTag);
	HRESULT Update_Timer(const wstring& pTimerTag);

private:
	map<const wstring, class CTimer*>			m_Timers;
	typedef map<const wstring, class CTimer*>	TIMERS;

private:
	class CTimer* Find_Timer(const wstring& pTimerTag);

public:	
	virtual void Free() override;
};

END
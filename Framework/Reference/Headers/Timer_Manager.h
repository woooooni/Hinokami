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
	/* Ÿ�ӵ�Ÿ�� ���´�. */
	/* Ÿ�ӵ�Ÿ�� ����Ѵ�. */
	_float Compute_TimeDelta(const wstring& strTimerTag);

public:
	HRESULT			Add_Timer(const wstring& strTimerTag);

private:
	map<const wstring, class CTimer*>		m_Timers;

private:
	class CTimer* Find_Timer(const wstring& strTimerTag) const;

public:
	virtual void	Free(void);
};

END
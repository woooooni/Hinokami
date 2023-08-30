#include "..\Public\Timer_Manager.h"
#include "Timer.h"

IMPLEMENT_SINGLETON(CTimer_Manager)

CTimer_Manager::CTimer_Manager()
{
}

_float CTimer_Manager::Get_TimeDelta(const wstring& strTimerTag)
{
	CTimer*		pTimer = Find_Timer(strTimerTag);
	if (nullptr == pTimer)
		return 0.0f;

	return pTimer->Get_TimeDelta();	
}

HRESULT CTimer_Manager::Add_Timer(const wstring& strTimerTag)
{
	if (nullptr != Find_Timer(strTimerTag))
		return E_FAIL;

	CTimer*		pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_Timers.emplace(strTimerTag, pTimer);

	return S_OK;
}

HRESULT CTimer_Manager::Update_Timer(const wstring& strTimerTag)
{
	CTimer*		pTimer = Find_Timer(strTimerTag);
	if (nullptr == pTimer)
		return E_FAIL;

	pTimer->Update();	

	return S_OK;
}

CTimer * CTimer_Manager::Find_Timer(const wstring& strTimerTag)
{
	auto	iter = find_if(m_Timers.begin(), m_Timers.end(), CTag_Finder(strTimerTag));

	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;
}

void CTimer_Manager::Free()
{
	for (auto& Pair : m_Timers)
		Safe_Release(Pair.second);

	m_Timers.clear();
}

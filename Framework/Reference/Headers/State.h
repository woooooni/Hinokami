#pragma once
#include "Base.h"



BEGIN(Engine)

class ENGINE_DLL CState abstract : CBase
{
public:
	CState(const wstring& strStateName, const wstring& strAnimationKey);
	CState(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState() = default;

public:
	virtual void Enter_State() PURE;
	virtual void Tick_State(_float fTimeDelta) PURE;
	virtual void Exit_State() PURE;

public:
	void Set_StateMachine(class CStateMachine* pStateMachine) { m_pStateMachine = pStateMachine; }

public:
	const wstring& Get_StateName() { return m_strStateName; }
	void Set_StateName(const wstring& strName) { m_strStateName = strName; }

	const wstring& Get_AnimationKey() { return m_strAnimationKey; }
	void Set_AnimationKey(const wstring& strAnimationKey) { m_strAnimationKey = strAnimationKey; }

protected:
	wstring m_strStateName = L"";
	wstring m_strAnimationKey = L"";

private:
	class CStateMachine* m_pStateMachine = nullptr;

public:
	void Free() override;
};

END


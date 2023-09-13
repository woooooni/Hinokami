#pragma once
#include "Base.h"



BEGIN(Engine)

class ENGINE_DLL CState abstract
{
public:
	CState();
	virtual ~CState() = default;

public:
	virtual void Enter_State() PURE;
	virtual void Tick_State(_float fTimeDelta) PURE;
	virtual void Exit_State() PURE;

public:
	void Set_StateMachine(class CStateMachine* pStateMachine) { m_pStateMachine = pStateMachine; }

private:
	class CStateMachine* m_pStateMachine = nullptr;
};

END


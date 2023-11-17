#pragma once

#include "Client_Defines.h"
#include "State.h"
BEGIN(Engine)
class CGameObject;
class CModel;
class CNavigation;
END

BEGIN(Client)

class CState_Monster_Defence_Trace final : public CState
{
private:
	CState_Monster_Defence_Trace(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine);
	virtual ~CState_Monster_Defence_Trace() = default;

public:
	HRESULT Initialize(const list<wstring>& AnimationList);

public:
	virtual void Enter_State(void* pArg = nullptr) override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

public:
	class CNavigation* m_pNavigation = nullptr;	
	class CGameObject* m_pTarget = nullptr;

private:
	void Find_NearTarget();


public:
	static CState_Monster_Defence_Trace* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const list<wstring>& AnimationList);
	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "State.h"
BEGIN(Engine)
class CModel;
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CState_NormalMonster0_Attack final : public CState
{

private:
	CState_NormalMonster0_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine);
	virtual ~CState_NormalMonster0_Attack() = default;

public:
	virtual HRESULT Initialize(const list<wstring>& AnimationList) override;

public:
	virtual void Enter_State(void* pArg = nullptr) override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;
	
public:
	static CState_NormalMonster0_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const list<wstring>& AnimationList);
	virtual void Free() override;
};

END
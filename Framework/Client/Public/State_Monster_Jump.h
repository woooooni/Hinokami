#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)

class CRigidBody;
class CModel;

END

BEGIN(Client)

class CState_Monster_Jump final : public CState
{
private:
	CState_Monster_Jump(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine);
	virtual ~CState_Monster_Jump() = default;

public:
	virtual HRESULT Initialize(const list<wstring>& AnimationList) override;

public:
	virtual void Enter_State(void* pArg = nullptr) override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

public:
	static CState_Monster_Jump* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const list<wstring>& AnimationList);
	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)

class CRigidBody;
class CModel;

END

BEGIN(Client)

class CState_Tanjiro_Basic_Jump final : public CState
{
private:
	CState_Tanjiro_Basic_Jump(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine);
	virtual ~CState_Tanjiro_Basic_Jump() = default;

public:
	HRESULT Initialize(const list<wstring>& AnimationList);

public:
	virtual void Enter_State(void* pArg = nullptr) override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CCharacter* m_pCharacter = nullptr;
	class CSword* m_pSword = nullptr;


public:
	static CState_Tanjiro_Basic_Jump* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const list<wstring>& AnimationList);
	virtual void Free() override;
};

END
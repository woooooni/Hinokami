#pragma once

#include "Client_Defines.h"
#include "State.h"
BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CState_Tanjiro_Basic_Idle final : public CState
{
private:
	CState_Tanjiro_Basic_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine);
	virtual ~CState_Tanjiro_Basic_Idle() = default;

public:
	HRESULT Initialize(const list<wstring>& AnimationList);

private:
	class CCharacter* m_pCharacter = nullptr;
	class CSword* m_pSword = nullptr;

public:
	virtual void Enter_State(void* pArg = nullptr) override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


public:
	static CState_Tanjiro_Basic_Idle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const list<wstring>& AnimationList);
	virtual void Free() override;
};

END
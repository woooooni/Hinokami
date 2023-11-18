#pragma once

#include "Client_Defines.h"
#include "State.h"
BEGIN(Engine)
class CRigidBody;
END

BEGIN(Client)

class CState_Character_Damaged_AirBorn final : public CState
{
private:
	CState_Character_Damaged_AirBorn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine);
	virtual ~CState_Character_Damaged_AirBorn() = default;

public:
	HRESULT Initialize(const list<wstring>& AnimationList);

public:
	virtual void Enter_State(void* pArg = nullptr) override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CCharacter* m_pOwnerCharacter = nullptr;

	_bool m_bFirstGround = false;
	_float m_fAccRecovery = 0.f;
	_float m_fRecoveryTime = 3.f;

	

public:
	static CState_Character_Damaged_AirBorn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const list<wstring>& AnimationList);
	virtual void Free() override;
};

END
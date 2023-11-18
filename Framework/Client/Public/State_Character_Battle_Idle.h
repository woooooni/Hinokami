#pragma once

#include "Client_Defines.h"
#include "State.h"
BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CState_Character_Battle_Idle final : public CState
{
private:
	CState_Character_Battle_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,class CStateMachine* pStateMachine);
	virtual ~CState_Character_Battle_Idle() = default;

public:
	HRESULT Initialize(const list<wstring>& AnimationList);

public:
	virtual void Enter_State(void* pArg = nullptr) override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


private:
	class CCharacter* m_pCharacter = nullptr;
	class CSword* m_pSword = nullptr;

	_float m_fBaseNutTime = 5.f;
	_float m_fAccBaseNut = 0.f;
	
public:
	static CState_Character_Battle_Idle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const list<wstring>& AnimationList);
	virtual void Free() override;
};

END
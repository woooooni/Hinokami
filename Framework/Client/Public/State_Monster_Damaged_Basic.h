#pragma once

#include "Client_Defines.h"
#include "State.h"
BEGIN(Engine)
class CModel;
END

BEGIN(Client)
class CMonster;
class CState_Monster_Damaged_Basic final : public CState
{
private:
	CState_Monster_Damaged_Basic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine);
	virtual ~CState_Monster_Damaged_Basic() = default;

public:
	virtual HRESULT Initialize(const list<wstring>& AnimationList) override;

public:
	virtual void Enter_State(void* pArg = nullptr) override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CMonster* m_pOwnerMonster = nullptr;

public:
	static CState_Monster_Damaged_Basic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CStateMachine* pStateMachine, const list<wstring>& AnimationList);
	virtual void Free() override;
};

END
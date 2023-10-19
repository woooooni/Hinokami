#pragma once

#include "Client_Defines.h"
#include "State.h"
BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CState_Tanjiro_Idle final : public CState
{
private:
	CState_Tanjiro_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CTransform* pTransform, class CStateMachine* pStateMachine, class CModel* pModel);
	virtual ~CState_Tanjiro_Idle() = default;

public:
	HRESULT Initialize(const list<wstring>& AnimationList);

public:
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


public:
	static CState_Tanjiro_Idle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CTransform* pTransform, class CStateMachine* pStateMachine, class CModel* pModel, const list<wstring>& AnimationList);
	virtual void Free() override;
};

END
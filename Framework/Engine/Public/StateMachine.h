#pragma once
#include "Component.h"


BEGIN(Engine)

class ENGINE_DLL CStateMachine final : public CComponent
{
private:
	CStateMachine(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CStateMachine(const CStateMachine& rhs);
	virtual ~CStateMachine() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Tick_State(_float fTimeDelta);

public:
	HRESULT Add_State(_uint eStateTag, class CState* pState);
	HRESULT Change_State(_uint eStateTag, void* pArg = nullptr);

	_uint Get_CurrState() { return m_iCurrState;}
		

private:
	_uint m_iCurrState = -1;
	class CState* m_pCurrState = nullptr;
	map<_uint, class CState*> m_States;

private:
	class CState* Find_State(const _uint eState);


public:
	static CStateMachine* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();
};

END


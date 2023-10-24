#pragma once
#include "Base.h"



BEGIN(Engine)

class ENGINE_DLL CState abstract : public CBase
{
protected:
	CState(class CStateMachine* pStateMachine);
	virtual ~CState() = default;


public:
	virtual void Enter_State(void* pArg = nullptr) PURE;
	virtual void Tick_State(_float fTimeDelta) PURE;
	virtual void Exit_State() PURE;


protected:
	_uint m_iCurrAnimIndex = 0;
	vector<_uint> m_AnimationIndices;

protected:
	class CStateMachine* m_pStateMachineCom = nullptr;
	class CModel* m_pModelCom = nullptr;
	class CTransform* m_pTransformCom = nullptr;
	
public:
	virtual void Free() override;
};

END


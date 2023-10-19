#pragma once
#include "Base.h"



BEGIN(Engine)

class ENGINE_DLL CState abstract : public CBase
{
protected:
	CState(class CStateMachine* pStateMachine, class CModel* pModel, class CTransform* pTransform);
	virtual ~CState() = default;


public:
	virtual void Enter_State(void* pArg = nullptr) PURE;
	virtual void Tick_State(_float fTimeDelta) PURE;
	virtual void Exit_State() PURE;


protected:
	class CModel* m_pModelCom = nullptr;
	class CStateMachine* m_pStateMachineCom = nullptr;
	class CTransform* m_pTransformCom = nullptr;
	_uint m_iCurrAnimIndex = 0;
	vector<_uint> m_AnimationIndices;
	
public:
	virtual void Free() override;
};

END


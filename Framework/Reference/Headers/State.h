#pragma once
#include "Base.h"



BEGIN(Engine)

class ENGINE_DLL CState abstract : public CBase
{
protected:
	CState(class CStateMachine* pStateMachine);
	virtual ~CState() = default;


public:
	virtual HRESULT Initialize(const list<wstring>&AnimationList);

public:
	virtual void Enter_State(void* pArg = nullptr) PURE;
	virtual void Tick_State(_float fTimeDelta) PURE;
	virtual void Exit_State() PURE;


protected:
	vector<_uint> m_AnimIndices;
	_int m_iCurrAnimIndex = -1;

protected:
	class CGameObject* m_pOwner = nullptr;
	class CModel* m_pModelCom = nullptr;
	class CTransform* m_pTransformCom = nullptr;
	class CStateMachine* m_pStateMachineCom = nullptr;
	class CNavigation* m_pNavigationCom = nullptr;
	class CRigidBody* m_pRigidBodyCom = nullptr;
	
	
	
public:
	virtual void Free() override;
};

END


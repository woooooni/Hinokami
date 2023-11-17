#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CRigidBody;
class CPipeLine;
class CStateMachine;
class CNavigation;
class CHierarchyNode;
END


BEGIN(Client)
class CNpc abstract : public CGameObject
{

public:
#pragma region NPC_STATES
	enum NPC_STATE { 
		IDLE,
		WALK,
		TALK,
		STATE_END
	};
#pragma endregion


protected:
	CNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	CNpc(const CNpc& rhs);
	virtual ~CNpc() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	HRESULT Render_ShadowDepth();

public:
	virtual void Collision_Enter(const COLLISION_INFO& tInfo) override;
	virtual void Collision_Continue(const COLLISION_INFO& tInfo) override;
	virtual void Collision_Exit(const COLLISION_INFO& tInfo) override;
	virtual void Talk() {};

protected:
	virtual HRESULT Ready_Components()	 PURE;
	virtual HRESULT Ready_States()		PURE;
	virtual HRESULT Ready_Colliders() PURE;


public:
	virtual void On_Damaged(CGameObject* pAttacker, _uint eDamageType, _float fDamage) {};
	


protected: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CRigidBody* m_pRigidBodyCom = nullptr;
	CStateMachine* m_pStateCom = nullptr;
	CNavigation* m_pNavigationCom = nullptr;

	CTexture* m_pDissoveTexture = nullptr;

protected:
	_float m_fDissolveWeight = 0.f;


public:
	virtual void Free() override;


};

END


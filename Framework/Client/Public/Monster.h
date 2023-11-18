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
class CMonster abstract : public CGameObject
{

public:
#pragma region MONSTER_STATES
	enum MONSTER_STATE { 
		REGEN,
		IDLE, 
		TRACE,  
		DEFENCE_TRACE,
		JUMP,		
		ATTACK,
		SKILL,
		DAMAGED_BASIC, 
		DAMAGED_BLOW,
		DAMAGED_AIRBORN,
		DAMAGED_AIRSTAY,
		DAMAGED_BOUND,
		KNOCKDOWN,
		DIE,
		STATE_END
	};

#pragma endregion
	enum SOCKET_TYPE { SOCKET_LEFT_FIST, SOCKET_RIGHT_FIST, SOCKET_LEFT_FOOT, SOCKET_RIGHT_FOOT, SOCKET_END };

public:
	typedef struct tagMonsterStat
	{
		_float fHp = 0.f;
		_float fMp = 0.f;
	} MONSTER_STAT;

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const MONSTER_STAT& tStat);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

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

public:
	virtual void Set_Infinite(_float fInfiniteTime, _bool bInfinite)
	{
		m_bInfinite = bInfinite;
		m_fInfiniteTime = fInfiniteTime;
		m_fAccInfinite = 0.f;
	}
	_bool Is_Infinite() { return m_bInfinite; }

	


public:
	CHierarchyNode* Get_Socket(const wstring& strSocketName);

protected:
	virtual HRESULT Ready_Components() PURE;
	virtual HRESULT Ready_States() PURE;
	virtual HRESULT Ready_Colliders() PURE;


public:
	virtual void On_Damaged(const COLLISION_INFO& tInfo);
	


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
	vector<CGameObject*>				m_Parts;
	typedef vector<CGameObject*>		PARTS;

	vector<class CHierarchyNode*>		m_Sockets;
	typedef vector<CGameObject*>		Sockets;

protected:
	MONSTER_STAT m_tStat = {};
	
	_float m_fAccInfinite = 0.f;
	_float m_fInfiniteTime = 0.2f;
	_bool m_bInfinite = false;

protected:
	_float m_fDissolveWeight = 0.f;


public:
	virtual void Free() override;


};

END


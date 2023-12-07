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
		JUMP,
		ATTACK,
		BOSS_ATTACK_0,
		BOSS_ATTACK_1,
		BOSS_ATTACK_2,
		SKILL_0,
		SKILL_1,
		DASH,
		DAMAGED_BASIC, 
		DAMAGED_BLOW,
		DAMAGED_AIRBORN,
		DAMAGED_BOUND,
		KNOCKDOWN,
		DIE,
		TRACE,
		DEFENCE_TRACE,
		STATE_END
	};

#pragma endregion
	enum SOCKET_TYPE { SOCKET_LEFT_FIST, SOCKET_RIGHT_FIST, SOCKET_LEFT_FOOT, SOCKET_RIGHT_FOOT, SOCKET_END };
	enum MONSTER_TYPE { NORMAL_0, NORMAL_1, NORMAL_2, ENMU, AKAZA, TYPE_END };
public:
	typedef struct tagMonsterStat
	{
		_float fHp = 100.f;
		_float fMp = 0.f;

		
		_float fMaxHp = 100.f;
		_float fMaxMp = 10.f;
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
	const MONSTER_STAT& Get_Stat() { return m_tStat; }


public:
	virtual void On_Damaged(const COLLISION_INFO& tInfo);
	
public:
	MONSTER_TYPE Get_Monster_Type() { return m_eMonsterType; }

protected:
	virtual HRESULT Ready_Components() PURE;
	virtual HRESULT Ready_States() PURE;
	virtual HRESULT Ready_Colliders() PURE;

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
	class CTrail* m_pTrails[SOCKET_TYPE::SOCKET_END];

protected:
	_float m_fDissolveWeight = 0.f;
	MONSTER_TYPE m_eMonsterType = MONSTER_TYPE::TYPE_END;

private:
	void LookAt_DamagedObject(CGameObject* pAttacker);
	void Play_DamagedSound();


public:
	virtual void Free() override;


};

END


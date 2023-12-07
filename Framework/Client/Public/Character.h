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
class CTrail;
class CCharacter abstract : public CGameObject
{

public:
#pragma region CHARACTER_STATES
	enum STATE { 
		BASIC_IDLE, 
		BASIC_MOVE,  
		BASIC_JUMP,
		BATTLE_IDLE, 
		BATTLE_MOVE, 
		BATTLE_JUMP, 
		BATTLE_DASH,
		BATTLE_AIRDASH,
		ATTACK,
		AIR_ATTACK,
		SKILL_0,
		SKILL_1,
		SKILL_2,
		SPECIAL,
		TRY_SPECIAL,
		DAMAGED_BASIC,
		DAMAGED_BOUND,
		DAMAGED_BLOW,
		DAMAGED_AIRBORN,
		KNOCKDOWN,
		DIE,
		STATE_END
	};

#pragma endregion

	enum PARTTYPE { PART_SWEATH, PART_SWORD, PART_END };
	enum SOCKET_TYPE { SOCKET_SWORD, SOCKET_SWEATH, SOCKET_RIGHT_HAND, SOCKET_LEFT_FOOT, SOCKET_RIGHT_FOOT, SOCKET_END };
	enum CHARACTER_TYPE { TANJIRO, ZENITSU, KYOJURO, ENMU, AKAZA, CHARACTER_END };
public:
	typedef struct tagCharacterStat
	{
		_float fHp = 100.f;
		_float fMp = 100.f;

		_float fMaxHp = 100.f;
		_float fMaxMp = 100.f;
	}CHARACTER_STAT;

protected:
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, CHARACTER_TYPE eCharacterType);
	CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();

public:
	virtual void Collision_Enter(const COLLISION_INFO& tInfo) override;
	virtual void Collision_Continue(const COLLISION_INFO& tInfo) override;
	virtual void Collision_Exit(const COLLISION_INFO& tInfo) override;


public:
	CHierarchyNode* Get_Socket(PARTTYPE eType);
	CHierarchyNode* Get_Socket(const wstring& strSocketName);

	template<class T>
	T* Get_Part(PARTTYPE eType) 
	{ 
		if (m_Parts[eType] == nullptr)
			return nullptr;

		return dynamic_cast<T*>(m_Parts[eType]);
	};

public:
	void DrawSword();
	void SweathSword();

	void Generate_Trail(SOCKET_TYPE eSocketType);
	void Stop_Trail(SOCKET_TYPE eSocketType);

	void Play_Sound(CCollider::ATTACK_TYPE eAttackType);
public:
	virtual void Set_Infinite(_float fInfiniteTime, _bool bInfinite);
	_bool Is_Infinite() { return m_bInfinite; }

public:
	const CHARACTER_STAT& Get_Stat() { return m_tStat; }
	void Set_Stat(const CHARACTER_STAT& StatDesc) { m_tStat = StatDesc; }
	void LookAt_DamagedObject(CGameObject* pAttacker);

public:
	CHARACTER_TYPE Get_CharacterType() { return m_eCharacterType; }

protected:
	virtual HRESULT Ready_Components() PURE;
	virtual HRESULT Ready_States() PURE;
	virtual HRESULT Ready_Colliders() PURE;
	virtual void On_Damaged(const COLLISION_INFO& tInfo);

	



protected: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CRigidBody* m_pRigidBodyCom = nullptr;
	CStateMachine* m_pStateCom = nullptr;
	CNavigation* m_pNavigationCom = nullptr;

	class CTrail* m_pTrails[SOCKET_END];


protected:
	vector<CGameObject*>				m_Parts;
	typedef vector<CGameObject*>		PARTS;

	vector<class CHierarchyNode*>		m_Sockets;
	typedef vector<CGameObject*>		Sockets;

protected:
	_float m_fAccInfinite = 0.f;
	_float m_fInfiniteTime = 5.f;
	_bool m_bInfinite = false;

	CHARACTER_STAT m_tStat = {};
	CHARACTER_TYPE m_eCharacterType = CHARACTER_TYPE::CHARACTER_END;

	

public:
	virtual void Free() override;


};

END


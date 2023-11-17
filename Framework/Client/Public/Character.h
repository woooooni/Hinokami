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
	enum SOCKET_TYPE { SOCKET_SWORD, SOCKET_SWEATH, SOCKET_RIGHT_HAND, SOCKET_LEFT_FOOT, SOCKET_RIGHT_FOOT, SOCEKT_END };

protected:
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
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
	CShader* Get_ShaderCom() { return m_pShaderCom; }
	CTransform* Get_TransformCom() { return m_pTransformCom; }
	CModel* Get_ModelCom() { return m_pModelCom; }
	CStateMachine* Get_StateCom() { return m_pStateCom; }
	CRigidBody* Get_RigidBodyCom() { return m_pRigidBodyCom; }

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

public:
	virtual void Set_Infinite(_float fInfiniteTime, _bool bInfinite);
	_bool Is_Infinite() { return m_bInfinite; }




protected:
	virtual HRESULT Ready_Components() PURE;
	virtual HRESULT Ready_States() PURE;
	virtual HRESULT Ready_Colliders() PURE;


	virtual void On_Damaged(CGameObject* pAttacker) {};

protected: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CRigidBody* m_pRigidBodyCom = nullptr;
	CStateMachine* m_pStateCom = nullptr;
	CNavigation* m_pNavigationCom = nullptr;

	class CTrail* m_pTrails[SOCEKT_END];


protected:
	vector<CGameObject*>				m_Parts;
	typedef vector<CGameObject*>		PARTS;

	vector<class CHierarchyNode*>		m_Sockets;
	typedef vector<CGameObject*>		Sockets;

protected:
	_float m_fAccInfinite = 0.f;
	_float m_fInfiniteTime = 5.f;
	_bool m_bInfinite = false;
	

public:
	virtual void Free() override;


};

END


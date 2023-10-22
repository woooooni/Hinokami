#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CPipeLine;
class CStateMachine;
class CNavigation;
class CHierarchyNode;
END


BEGIN(Client)
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
		ATTACK,
		SKILL,
		DAMAGED, 
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

public:
	CShader* Get_ShaderCom() { return m_pShaderCom; }
	CTransform* Get_TransformCom() { return m_pTransformCom; }
	CModel* Get_ModelCom() { return m_pModelCom; }
	CStateMachine* Get_StateCom() { return m_pStateCom; }

public:
	CHierarchyNode* Get_Socket(PARTTYPE eType);
	CHierarchyNode* Get_Socket(const wstring& strSocketName);

public:
	void DrawSword();
	void SweathSword();

protected:
	virtual HRESULT Ready_Components() PURE;
	virtual HRESULT Ready_States() PURE;


protected: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CStateMachine* m_pStateCom = nullptr;
	CNavigation* m_pNavigationCom = nullptr;

protected:
	vector<CGameObject*>				m_Parts;
	typedef vector<CGameObject*>		PARTS;

	vector<class CHierarchyNode*>		m_Sockets;
	typedef vector<CGameObject*>		Sockets;


protected:
	void Input(_float fTimeDelta);
	

public:
	virtual void Free() override;


};

END


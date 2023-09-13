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
class CHierarchyNode;
END


BEGIN(Client)
class CCharacter abstract : public CGameObject
{
public:
#pragma region CHARACTER_STATES
	enum STATE { STATE_IDLE, STATE_WALK, STATE_RUN, STATE_DASH, STATE_JUMP, STATE_ATTACK, STATE_SKILL, STATE_DAMAGED, STATE_KNOCKDOWN, STATE_DIE, STATE_END };
#pragma endregion

	enum PARTTYPE { PART_WEAPON, PART_END };

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
	void Set_Controlable(_bool _bControlable) { m_bControlable = _bControlable; }
	void Set_MainCharacter(_bool _bMain) { m_bMainPlayable = _bMain; }
	_bool Is_Controlable() { return m_bControlable; }
	_bool Is_MainPlayable() { return m_bMainPlayable; }

protected:
	virtual HRESULT Ready_Components() PURE;
	virtual HRESULT Ready_States() PURE;

protected: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CStateMachine* m_pStateCom = nullptr;

protected:
	vector<CGameObject*>				m_Parts;
	typedef vector<CGameObject*>		PARTS;
	vector<class CHierarchyNode*>		m_Sockets;

protected:
	_bool m_bControlable = false;
	_bool m_bMainPlayable = false;


protected:
	void Input(_float fTimeDelta);
	

public:
	virtual void Free() override;


};

END


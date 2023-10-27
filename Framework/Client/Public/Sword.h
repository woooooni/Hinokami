#pragma once

#include "Client_Defines.h"
#include "Part.h"


BEGIN(Client)

class CSword final : public CPart
{
public:
	enum SWORD_MODE { BASIC, BLOW, BOUND, AIR_BONE, SPL_STRIKE, MODE_END };

public:
	typedef struct tagWeaponDesc : public CPart::PART_DESC
	{		
		_float3			vRotationDegree;
	} SWORD_DESC;

protected:
	/* ������ ������ �� */
	CSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	/* �纻�� ������ �� */
	CSword(const CSword& rhs); /* ���� ������. */

	virtual ~CSword() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strPrototypeSwordModel);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	void Set_SwordMode(SWORD_MODE eMode) { m_eSwordMode = eMode; }
	void Set_PushPower(_float fPower) { m_fPushPower = fPower; }
	_float Get_PushPower() { return m_fPushPower; }
	SWORD_MODE Get_SwordMode() { return m_eSwordMode; }

public:
	virtual void Collision_Enter(const COLLISION_INFO& tInfo) override;
	virtual void Collision_Continue(const COLLISION_INFO& tInfo) override;
	virtual void Collision_Exit(const COLLISION_INFO& tInfo) override;


public:
	void Generate_Trail();
	void Stop_Trail();
	void Generate_Effect();

private:
	wstring					m_strModelPrototype;
	SWORD_MODE				m_eSwordMode;
	_float					m_fPushPower = 0.f;


private:
	HRESULT Ready_Components();
	HRESULT Ready_Colliders();

	HRESULT Bind_ShaderResources();

public:
	static CSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const wstring& strPrototypeSwordModel);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END
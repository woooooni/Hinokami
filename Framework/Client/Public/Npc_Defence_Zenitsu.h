#pragma once

#include "Client_Defines.h"
#include "Npc.h"



BEGIN(Client)

class CNpc_Defence_Zenitsu final : public CNpc
{

private:
	CNpc_Defence_Zenitsu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	CNpc_Defence_Zenitsu(const CNpc_Defence_Zenitsu& rhs);
	virtual ~CNpc_Defence_Zenitsu() = default;

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

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Ready_States() override;
	virtual HRESULT Ready_Colliders() override;
	HRESULT Ready_Defence();
	HRESULT Ready_Scripts();

	virtual void Talk() override;
	virtual void Talking(_float fTimeDelta);

private:
	void Start_Defence();
	void Tick_Defence(_float fTimeDelta);

private:
	vector<_float4> m_RegenPositions;
	vector<_uint> m_iRandomAnim_Indices;

	_uint m_iDefenceDifficulty = 0;
	_bool m_bClearDefence = false;
	_bool m_bStartDefence = false;

	

	_float m_fAccRandomAnim = 0.f;
	_float m_fChangeAnimTime = 5.f;

	_uint m_iTalkIdx = 0;
	_bool m_bTalking = false;
	
	_bool m_bReserveNextScene = false;
	_float m_fTextAlpha = 1.f;

private:
	vector<wstring> m_StartScripts;
	vector<wstring> m_EndScripts;


public:
	virtual void On_Damaged(CGameObject* pAttacker, _uint eDamageType, _float fDamage) override;


public:
	static CNpc_Defence_Zenitsu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};

END


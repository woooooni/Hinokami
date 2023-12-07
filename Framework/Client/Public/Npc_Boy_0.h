#pragma once

#include "Client_Defines.h"
#include "Npc.h"



BEGIN(Client)

class CNpc_Boy_0 final : public CNpc
{

private:
	CNpc_Boy_0(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNpc_Boy_0(const CNpc_Boy_0& rhs);
	virtual ~CNpc_Boy_0() = default;

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

public:
	virtual void On_Damaged(CGameObject* pAttacker, _uint eDamageType, _float fDamage) override;


public:
	static CNpc_Boy_0* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};

END


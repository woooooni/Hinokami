#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CAkaza_Projectile final : public CGameObject
{
private:
	CAkaza_Projectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAkaza_Projectile(const CAkaza_Projectile& rhs);
	virtual ~CAkaza_Projectile() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	virtual void Collision_Enter(const COLLISION_INFO & tInfo) override;
	virtual void Collision_Continue(const COLLISION_INFO & tInfo) override;
	virtual void Collision_Exit(const COLLISION_INFO & tInfo) override;


private:
	class CTransform* m_pTransformCom = nullptr;
	class CRenderer* m_pRendererCom = nullptr;
private:
	_float m_fSpeed = 10.f;
	_float m_fAccDeletionTime = 0.f;
	_float m_fDeletionTime = 5.f;

private:
	HRESULT Ready_Components();
	HRESULT Ready_Colliders();
	HRESULT Bind_ShaderResources();



public:
	static CAkaza_Projectile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;

};

END
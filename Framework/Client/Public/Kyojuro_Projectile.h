#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CKyojuro_Projectile final : public CGameObject
{
private:
	/* 원형을 생성할 때 */
	CKyojuro_Projectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CKyojuro_Projectile(const CKyojuro_Projectile& rhs); /* 복사 생성자. */
	virtual ~CKyojuro_Projectile() = default;

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
	_float m_fSpeed = 5.f;
	_float m_fAccDeletionTime = 0.f;
	_float m_fDeletionTime = 2.5f;

private:
	_float m_fAccGenParticle = 0.f;
	_float m_fGenParticleTime = 0.1f;

private:
	HRESULT Ready_Components();
	HRESULT Ready_Colliders();
	HRESULT Bind_ShaderResources();



public:
	static CKyojuro_Projectile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;

};

END
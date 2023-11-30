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

class CEnmu_Projectile final : public CGameObject
{
private:
	/* ������ ������ �� */
	CEnmu_Projectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CEnmu_Projectile(const CEnmu_Projectile& rhs); /* ���� ������. */
	virtual ~CEnmu_Projectile() = default;

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
	_float m_fDeletionTime = 10.f;

private:
	HRESULT Ready_Components();
	HRESULT Ready_Colliders();
	HRESULT Bind_ShaderResources();



public:
	static CEnmu_Projectile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;

};

END
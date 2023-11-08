#pragma once
#include "Component.h"



BEGIN(Engine)

class ENGINE_DLL CCollider_Sphere final : public CCollider
{

public:
    typedef struct tagSphereColliderDesc : COLLIDER_DESC
    {
        BoundingSphere tSphere;
    } SPHERE_COLLIDER_DESC;

private:
    CCollider_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCollider_Sphere(CCollider_Sphere& rhs);
    virtual ~CCollider_Sphere() = default;


public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);

public:
    virtual _bool Is_Collision(CCollider* pCollider) override;
    virtual void Collision_Enter(CCollider* pCollider) override;
    virtual void Collision_Continue(CCollider* pCollider) override;
    virtual void Collision_Exit(CCollider* pCollider) override;

    virtual _vector Get_Position() override { return XMLoadFloat3(&m_tBoundingSphere.Center); }
    virtual _float Get_Radius() override { return m_tBoundingSphere.Radius; }

public:
    void LateTick_Collider(_float fTimeDelta) override;
    HRESULT Render() override;

public:
    const BoundingSphere& Get_Sphere() { return m_tBoundingSphere; }
    void Set_Sphere(const BoundingSphere& tSphere) { m_tBoundingSphere = tSphere; }


private:
    BoundingSphere m_tBoundingSphere = {};

public:
    static CCollider_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;

};
END

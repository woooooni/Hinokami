#pragma once
#include "Collider.h"



BEGIN(Engine)

class ENGINE_DLL CCollider_AABB final : public CCollider
{

public:
    typedef struct tagSphereColliderDesc : COLLIDER_DESC
    {
        BoundingBox tBox;
    } AABB_COLLIDER_DESC;

private:
    CCollider_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCollider_AABB(CCollider_AABB& rhs);
    virtual ~CCollider_AABB() = default;


public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);

public:
    virtual _bool Is_Collision(CCollider* pCollider) override;
    virtual void Collision_Enter(CCollider* pCollider) override;
    virtual void Collision_Continue(CCollider* pCollider) override;
    virtual void Collision_Exit(CCollider* pCollider) override;

    virtual _vector Get_Position() override { return XMLoadFloat3(&m_tBoundingBox.Center); }



public:
    void LateTick_Collider(_float fTimeDelta) override;
    HRESULT Render() override;

public:
    void Set_AABB_Box(BoundingBox& tBoundingBox) { m_tBoundingBox = tBoundingBox; }
    const BoundingBox& Get_AABB_Box() { return m_tBoundingBox; }


private:
    BoundingBox m_tBoundingBox = {};

public:
    static CCollider_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;

};
END

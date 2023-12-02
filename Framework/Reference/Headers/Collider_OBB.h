#pragma once
#include "Collider.h"



BEGIN(Engine)

class ENGINE_DLL CCollider_OBB final : public CCollider
{

public:
    typedef struct tagSphereColliderDesc : COLLIDER_DESC
    {
        BoundingOrientedBox tBox;
    } OBB_COLLIDER_DESC;

private:
    CCollider_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCollider_OBB(CCollider_OBB& rhs);
    virtual ~CCollider_OBB() = default;


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
    void Set_OBB_Box(BoundingOrientedBox& tBoundingBox) { m_tOriginOBB = tBoundingBox; }
    const BoundingOrientedBox& Get_OBB_Box() { return m_tBoundingBox; }


private:
    BoundingOrientedBox m_tOriginOBB = {};
    BoundingOrientedBox m_tBoundingBox = {};

public:
    static CCollider_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;

};
END

#pragma once
#include "Component.h"



BEGIN(Engine)

class ENGINE_DLL CCollider abstract : public CComponent
{
public:
    enum COLLIDER_TYPE { SPHERE, AABB, TYPE_END };
    enum DETECTION_TYPE { BOUNDARY, HEAD, BODY, WEAPON, EFFECT, DETECTION_END };

protected:
    typedef struct tagColliderDesc
    {
        class CHierarchyNode* pNode;
        class CTransform* pOwnerTransform;
        _float4x4 ModePivotMatrix;
        _float3 vOffsetPosition = { 0.f, 0.f, 0.f };

    } COLLIDER_DESC;

protected:
    CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER_TYPE eColliderType);
    CCollider(CCollider& rhs);
    virtual ~CCollider() = default;


public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);

public:
    void Set_Active(_bool bActive) { m_bActive = bActive; }
    _bool Is_Active() { return m_bActive; }

    void Set_Offset(_float3 vOffset) { m_vOffsetPosition = vOffset; }
    const _float3 Get_Offset() { return m_vOffsetPosition; }

    COLLIDER_TYPE Get_ColliderType() { return m_eColliderType; }
    _uint Get_ColliderID() { return m_iColliderID; }

public:
    virtual _bool Is_Collision(CCollider* pCollider) PURE;
    virtual void Collision_Enter(CCollider* pCollider);
    virtual void Collision_Continue(CCollider* pCollider);
    virtual void Collision_Exit(CCollider* pCollider);


public:
    virtual void LateTick_Collider(_float fTimeDelta);
    HRESULT Render() PURE;
    
protected:
    static _uint g_iNextID;
    _uint m_iColliderID;
    
    DETECTION_TYPE m_eDetectionType = DETECTION_END;
    COLLIDER_TYPE m_eColliderType = COLLIDER_TYPE::TYPE_END;

    _float3 m_vOffsetPosition = { 0.f, 0.f, 0.f };
    _bool m_bActive = true;

protected:
    _float4x4 m_FinalMatrix = {};
    _float4x4 m_ModelPivotMatrix = {};

protected:
    class CHierarchyNode* m_pNode = nullptr;
    class CTransform* m_pOwnerTransformCom = nullptr;



protected:
    class PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
    class BasicEffect* m_pEffect = nullptr;
    ID3D11InputLayout* m_pInputLayout = nullptr;

    _float4	m_vColor = _float4(0.f, 1.f, 0.f, 1.f);
    _bool m_bDraw = true;

public:
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;

};
END

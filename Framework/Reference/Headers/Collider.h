#pragma once
#include "Component.h"



BEGIN(Engine)

class ENGINE_DLL CCollider abstract : public CComponent
{
public:
    enum COLLIDER_TYPE { SPHERE, AABB, OBB, TYPE_END };
    enum DETECTION_TYPE { BOUNDARY, ATTACK, BODY, HEAD, DETECTION_END };
    enum ATTACK_TYPE { BASIC, BLOW, BOUND, AIR_BORN, MODE_END };

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

    void Set_ColliderID(_uint iColliderID) { m_iColliderID = iColliderID; }
    _uint Get_ColliderID() { return m_iColliderID; }

    DETECTION_TYPE Get_DetectionType() { return m_eDetectionType; }
    void Set_DetectionType(DETECTION_TYPE eType) { m_eDetectionType = eType; }

    ATTACK_TYPE Get_AttackType() { return m_eAttackType; }
    void Set_AttackType(ATTACK_TYPE eType) { m_eAttackType = eType; }

    void Set_AirBorn_Power(_float fPower) { m_fAirBornPower = fPower; }
    _float Get_AirBorn_Power() { return m_fAirBornPower; }

    void Set_PushPower(_float fPower) { m_fPushPower = fPower; }
    _float Get_PushPower() { return m_fPushPower; }

    void Set_Damage(_float fDamage) { m_fDamage = fDamage; }
    _float Get_Damage() { return m_fDamage; }

    _bool Is_HitLag() { return m_bHitLag; }
    void Set_HitLag(_bool bHitLag) { m_bHitLag = bHitLag; }

    virtual _vector Get_Position() { return XMVectorSet(0.f, 0.f, 0.f, 0.f); }
    virtual _float Get_Radius() { return 0.f; }
    virtual _float3 Get_Extents() { return _float3(0.f, 0.f, 0.f); }

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
    
    ATTACK_TYPE m_eAttackType = BASIC;
    DETECTION_TYPE m_eDetectionType = DETECTION_END;
    COLLIDER_TYPE m_eColliderType = COLLIDER_TYPE::TYPE_END;

    _float3 m_vOffsetPosition = { 0.f, 0.f, 0.f };
    _bool m_bActive = true;
    _bool m_bHitLag = true;


protected:
    _float m_fDamage = 0.f;
    _float m_fAirBornPower = 0.f;
    _float m_fPushPower = 0.f;

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

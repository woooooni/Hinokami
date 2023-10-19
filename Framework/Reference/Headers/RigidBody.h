#pragma once
#include "Component.h"
class CRigidBody :
    public CComponent
{
private:
    CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRigidBody(CRigidBody& rhs);
    virtual ~CRigidBody() = default;

public:
    void Set_Gravity(_float fGravity) { m_fGravity = fGravity; }
    void Set_Gravity(_bool bGravity) { m_bIsGravity = bGravity; }

    _bool Is_Gravity() { return m_bIsGravity; }



private:
    _float m_fGravity = 9.8f;
    _float3 m_vVelocity = { 0.f, 0.f, 0.f };
    _float3 m_vFriction = { 0.f, 0.f, 0.f };

    _bool m_bIsGravity = true;

public:
    static CRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CTransform* pTransform);
    virtual CComponent* Clone(void* pArg = nullptr);
    virtual void Free() override;

};


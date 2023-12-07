#include "stdafx.h"
#include "GameInstance.h"
#include "Akaza_Skill_Boundary.h"
#include "Effect_Manager.h"
#include "Effect.h"

CAkaza_Skill_Boundary::CAkaza_Skill_Boundary(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext, L"Akaza_Projectile", OBJ_TYPE::OBJ_EFFECT)
{
}

CAkaza_Skill_Boundary::CAkaza_Skill_Boundary(const CAkaza_Skill_Boundary& rhs)
    : CGameObject(rhs)
{
}

HRESULT CAkaza_Skill_Boundary::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAkaza_Skill_Boundary::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Colliders()))
        return E_FAIL;

    if (FAILED(CEffect_Manager::GetInstance()->Generate_Effect(L"Akaza_Skill_Boundary", XMMatrixIdentity(), XMMatrixIdentity(), 999.f, this, &m_pEffect)))
        return E_FAIL;

    Safe_AddRef(m_pEffect);

    return S_OK;
}

void CAkaza_Skill_Boundary::Tick(_float fTimeDelta)
{

    __super::Tick(fTimeDelta);
    if (true == m_bDead)
    {
        m_pEffect->Set_Dead(true);
        return;
    }
}

void CAkaza_Skill_Boundary::LateTick(_float fTimeDelta)
{
    __super::LateTick(fTimeDelta);
    
    for (_uint i = 0; i < CCollider::DETECTION_TYPE::DETECTION_END; ++i)
    {
        for (auto& pCollider : m_Colliders[i])
            m_pRendererCom->Add_Debug(pCollider);
    }
    
}

HRESULT CAkaza_Skill_Boundary::Render()
{
    __super::Render();
    return S_OK;
}

void CAkaza_Skill_Boundary::Collision_Enter(const COLLISION_INFO& tInfo)
{
}

void CAkaza_Skill_Boundary::Collision_Continue(const COLLISION_INFO& tInfo)
{
}

void CAkaza_Skill_Boundary::Collision_Exit(const COLLISION_INFO& tInfo)
{
}

HRESULT CAkaza_Skill_Boundary::Ready_Components()
{
    /* For.Com_Transform */
    CTransform::TRANSFORMDESC		TransformDesc;
    ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

    TransformDesc.fSpeedPerSec = 5.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
        return E_FAIL;


    
    return S_OK;
}

HRESULT CAkaza_Skill_Boundary::Ready_Colliders()
{
    CCollider_Sphere::SPHERE_COLLIDER_DESC ColliderDesc;
    ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

    BoundingSphere tSphere;
    ZeroMemory(&tSphere, sizeof(BoundingSphere));
    tSphere.Radius = 1.f;

    ColliderDesc.pOwnerTransform = m_pTransformCom;

    XMStoreFloat4x4(&ColliderDesc.ModePivotMatrix, XMMatrixIdentity());
    ColliderDesc.tSphere = tSphere;
    ColliderDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::BOUNDARY, &ColliderDesc)))
        return E_FAIL;

    if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::ATTACK, &ColliderDesc)))
        return E_FAIL;

    Set_ActiveColliders(CCollider::DETECTION_TYPE::BOUNDARY, true);
    Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, true);

    Set_Collider_AttackMode(CCollider::ATTACK_TYPE::BASIC, 0.f, 6.f, 1.f);

    return S_OK;
}

HRESULT CAkaza_Skill_Boundary::Bind_ShaderResources()
{
    return S_OK;
}

CAkaza_Skill_Boundary* CAkaza_Skill_Boundary::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAkaza_Skill_Boundary* pInstance = new CAkaza_Skill_Boundary(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CAkaza_Skill_Boundary");
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

CGameObject* CAkaza_Skill_Boundary::Clone(void* pArg)
{
    CAkaza_Skill_Boundary* pInstance = new CAkaza_Skill_Boundary(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CAkaza_Skill_Boundary");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CAkaza_Skill_Boundary::Free()
{
    __super::Free();
    Safe_Release(m_pTransformCom);
}

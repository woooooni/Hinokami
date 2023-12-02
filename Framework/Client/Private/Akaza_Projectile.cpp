#include "stdafx.h"
#include "GameInstance.h"
#include "Akaza_Projectile.h"
#include "Effect_Manager.h"

CAkaza_Projectile::CAkaza_Projectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext, L"Akaza_Projectile", OBJ_TYPE::OBJ_EFFECT)
{
}

CAkaza_Projectile::CAkaza_Projectile(const CAkaza_Projectile& rhs)
    : CGameObject(rhs)
{
}

HRESULT CAkaza_Projectile::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAkaza_Projectile::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Colliders()))
        return E_FAIL;

    if (FAILED(CEffect_Manager::GetInstance()->Generate_Effect(L"Akaza_Projectile", XMMatrixIdentity(), XMMatrixIdentity(), 5.f, this)))
        return E_FAIL;

    return S_OK;
}

void CAkaza_Projectile::Tick(_float fTimeDelta)
{

    __super::Tick(fTimeDelta);
    GI->Add_CollisionGroup(COLLISION_GROUP::MONSTER, this);

    m_fAccDeletionTime += fTimeDelta;
    if(m_fAccDeletionTime >= m_fDeletionTime)
    {
        m_fAccDeletionTime = 0.f;
        Set_Dead(true);
        return;
    }

    _vector vLook = XMVector3Normalize(m_pTransformCom->Get_Look());
    m_pTransformCom->Go_Dir(vLook, m_fSpeed, fTimeDelta);

    
}

void CAkaza_Projectile::LateTick(_float fTimeDelta)
{
    __super::LateTick(fTimeDelta);
    
    for (_uint i = 0; i < CCollider::DETECTION_TYPE::DETECTION_END; ++i)
    {
        for (auto& pCollider : m_Colliders[i])
            m_pRendererCom->Add_Debug(pCollider);
    }
    
}

HRESULT CAkaza_Projectile::Render()
{
    __super::Render();
    return S_OK;
}

void CAkaza_Projectile::Collision_Enter(const COLLISION_INFO& tInfo)
{
}

void CAkaza_Projectile::Collision_Continue(const COLLISION_INFO& tInfo)
{
}

void CAkaza_Projectile::Collision_Exit(const COLLISION_INFO& tInfo)
{
}

HRESULT CAkaza_Projectile::Ready_Components()
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

HRESULT CAkaza_Projectile::Ready_Colliders()
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

HRESULT CAkaza_Projectile::Bind_ShaderResources()
{
    return S_OK;
}

CAkaza_Projectile* CAkaza_Projectile::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAkaza_Projectile* pInstance = new CAkaza_Projectile(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CAkaza_Projectile");
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

CGameObject* CAkaza_Projectile::Clone(void* pArg)
{
    CAkaza_Projectile* pInstance = new CAkaza_Projectile(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CAkaza_Projectile");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CAkaza_Projectile::Free()
{
    __super::Free();
    Safe_Release(m_pTransformCom);
}

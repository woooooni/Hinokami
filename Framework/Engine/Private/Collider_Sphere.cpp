#include "GameInstance.h"
#include "Collider_Sphere.h"
#include "HierarchyNode.h"
#include "Collider_AABB.h"
#include "Collider_OBB.h"
#include "GameObject.h"

CCollider_Sphere::CCollider_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCollider(pDevice, pContext, CCollider::SPHERE)

{
	
}

CCollider_Sphere::CCollider_Sphere(CCollider_Sphere& rhs)
	: CCollider(rhs)
{

}

HRESULT CCollider_Sphere::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CCollider_Sphere::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;

	SPHERE_COLLIDER_DESC* pDesc = static_cast<SPHERE_COLLIDER_DESC*>(pArg);
	m_tBoundingSphere = pDesc->tSphere;

	return S_OK;
}

_bool CCollider_Sphere::Is_Collision(CCollider* pCollider)
{
	if (false == m_bActive)
		return false;

	if (pCollider->Get_ColliderType() == CCollider::AABB)
	{
		CCollider_AABB* pOtherCollider = static_cast<CCollider_AABB*>(pCollider);
		return m_tBoundingSphere.Intersects(pOtherCollider->Get_AABB_Box());
	}
	else if (pCollider->Get_ColliderType() == CCollider::SPHERE)
	{
		CCollider_Sphere* pOtherCollider = static_cast<CCollider_Sphere*>(pCollider);
		return m_tBoundingSphere.Intersects(pOtherCollider->Get_Sphere());
	}

	else if (pCollider->Get_ColliderType() == CCollider::OBB)
	{
		CCollider_OBB* pOtherCollider = static_cast<CCollider_OBB*>(pCollider);
		return m_tBoundingSphere.Intersects(pOtherCollider->Get_OBB_Box());
	}

	return false;
}

void CCollider_Sphere::LateTick_Collider(_float fTimeDelta)
{
	__super::LateTick_Collider(fTimeDelta);
	XMStoreFloat3(&m_tBoundingSphere.Center, XMLoadFloat4x4(&m_FinalMatrix).r[CTransform::STATE_POSITION]);
}

HRESULT CCollider_Sphere::Render()
{
	/*if (m_bActive && m_eDetectionType != CCollider::BOUNDARY)
	{
		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(GAME_INSTANCE->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		m_pEffect->SetProjection(GAME_INSTANCE->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));


		m_pEffect->Apply(m_pContext);

		m_pContext->IASetInputLayout(m_pInputLayout);


		m_pBatch->Begin();

		DX::Draw(m_pBatch, m_tBoundingSphere, XMLoadFloat4(&m_vColor));


		m_pBatch->End();
	}*/
	if (false == m_bActive)
		return S_OK;

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(GAME_INSTANCE->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(GAME_INSTANCE->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));


	m_pEffect->Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);


	m_pBatch->Begin();

	DX::Draw(m_pBatch, m_tBoundingSphere, XMLoadFloat4(&m_vColor));


	m_pBatch->End();

	return S_OK;
}


void CCollider_Sphere::Collision_Enter(CCollider* pCollider)
{
	__super::Collision_Enter(pCollider);
}

void CCollider_Sphere::Collision_Continue(CCollider* pCollider)
{
	__super::Collision_Continue(pCollider);
}

void CCollider_Sphere::Collision_Exit(CCollider* pCollider)
{
	__super::Collision_Exit(pCollider);
}

CCollider_Sphere* CCollider_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCollider_Sphere* pInstance = new CCollider_Sphere(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Sphere Create Failed.");
		return nullptr;
	}
	return pInstance;
}

CComponent* CCollider_Sphere::Clone(void* pArg)
{
	CCollider_Sphere* pInstance = new CCollider_Sphere(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Sphere Clone Failed.");
		return nullptr;
	}
	return pInstance;
}

void CCollider_Sphere::Free()
{
		
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pNode);
	Safe_Release(m_pOwnerTransformCom);

	__super::Free();
}

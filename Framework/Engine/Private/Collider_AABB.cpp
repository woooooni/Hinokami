#include "GameInstance.h"
#include "Collider_AABB.h"
#include "Collider_Sphere.h"
#include "HierarchyNode.h"


CCollider_AABB::CCollider_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCollider(pDevice, pContext, CCollider::AABB)

{
	
}

CCollider_AABB::CCollider_AABB(CCollider_AABB& rhs)
	: CCollider(rhs)
{

}

HRESULT CCollider_AABB::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CCollider_AABB::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;

	AABB_COLLIDER_DESC* pDesc = static_cast<AABB_COLLIDER_DESC*>(pArg);
	m_tBoundingBox = pDesc->tBox;

	return S_OK;
}

_bool CCollider_AABB::Is_Collision(CCollider* pCollider)
{
	if (false == m_bActive)
		return false;

	if (pCollider->Get_ColliderType() == CCollider::AABB)
	{
		CCollider_AABB* pOtherCollider = static_cast<CCollider_AABB*>(pCollider);
		return m_tBoundingBox.Intersects(pOtherCollider->Get_AABB_Box());
	}
	else if (pCollider->Get_ColliderType() == CCollider::SPHERE)
	{
		CCollider_Sphere* pOtherCollider = static_cast<CCollider_Sphere*>(pCollider);
		return m_tBoundingBox.Intersects(pOtherCollider->Get_Sphere());
	}

	return false;
}



void CCollider_AABB::LateTick_Collider(_float fTimeDelta)
{
	__super::LateTick_Collider(fTimeDelta);
	XMStoreFloat3(&m_tBoundingBox.Center, XMLoadFloat4x4(&m_FinalMatrix).r[CTransform::STATE_POSITION]);
}

HRESULT CCollider_AABB::Render()
{
	//if (m_bActive/* && m_eDetectionType != CCollider::BOUNDARY*/)
	//{
	//	m_pEffect->SetWorld(XMMatrixIdentity());
	//	m_pEffect->SetView(GAME_INSTANCE->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	//	m_pEffect->SetProjection(GAME_INSTANCE->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	//	m_pEffect->Apply(m_pContext);

	//	m_pContext->IASetInputLayout(m_pInputLayout);


	//	m_pBatch->Begin();

	//	DX::Draw(m_pBatch, m_tBoundingBox, XMLoadFloat4(&m_vColor));
	//	m_pBatch->End();
	//}

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(GAME_INSTANCE->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(GAME_INSTANCE->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	m_pEffect->Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);


	m_pBatch->Begin();

	DX::Draw(m_pBatch, m_tBoundingBox, XMLoadFloat4(&m_vColor));
	m_pBatch->End();

	return S_OK;
}

void CCollider_AABB::Collision_Enter(CCollider* pCollider)
{
	__super::Collision_Enter(pCollider);
}

void CCollider_AABB::Collision_Continue(CCollider* pCollider)
{
	__super::Collision_Continue(pCollider);
}

void CCollider_AABB::Collision_Exit(CCollider* pCollider)
{
	__super::Collision_Exit(pCollider);
}


CCollider_AABB* CCollider_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCollider_AABB* pInstance = new CCollider_AABB(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Box Create Failed.");
		return nullptr;
	}
	return pInstance;
}

CComponent* CCollider_AABB::Clone(void* pArg)
{
	CCollider_AABB* pInstance = new CCollider_AABB(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Box Clone Failed.");
		return nullptr;
	}
	return pInstance;
}

void CCollider_AABB::Free()
{
		
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pNode);
	Safe_Release(m_pOwnerTransformCom);

	__super::Free();
}

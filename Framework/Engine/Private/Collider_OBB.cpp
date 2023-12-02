#include "GameInstance.h"
#include "Collider_AABB.h"
#include "Collider_OBB.h"
#include "Collider_Sphere.h"
#include "HierarchyNode.h"


CCollider_OBB::CCollider_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCollider(pDevice, pContext, CCollider::AABB)

{
	
}

CCollider_OBB::CCollider_OBB(CCollider_OBB& rhs)
	: CCollider(rhs)
{

}

HRESULT CCollider_OBB::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CCollider_OBB::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	OBB_COLLIDER_DESC* pDesc = static_cast<OBB_COLLIDER_DESC*>(pArg);
	m_tOriginOBB = pDesc->tBox;

	return S_OK;
}

_bool CCollider_OBB::Is_Collision(CCollider* pCollider)
{
	if (false == m_bActive)
		return false;

	if (pCollider->Get_ColliderType() == CCollider::OBB)
	{
		CCollider_OBB* pOtherCollider = static_cast<CCollider_OBB*>(pCollider);
		return m_tBoundingBox.Intersects(pOtherCollider->Get_OBB_Box());
	}
	else if (pCollider->Get_ColliderType() == CCollider::SPHERE)
	{
		CCollider_Sphere* pOtherCollider = static_cast<CCollider_Sphere*>(pCollider);
		return m_tBoundingBox.Intersects(pOtherCollider->Get_Sphere());
	}
	else if (pCollider->Get_ColliderType() == CCollider::AABB)
	{
		CCollider_AABB* pOtherCollider = static_cast<CCollider_AABB*>(pCollider);
		return m_tBoundingBox.Intersects(pOtherCollider->Get_AABB_Box());
	}

	return false;
}



void CCollider_OBB::LateTick_Collider(_float fTimeDelta)
{
	__super::LateTick_Collider(fTimeDelta);
	m_tOriginOBB.Transform(m_tBoundingBox, XMLoadFloat4x4(&m_FinalMatrix));
}

HRESULT CCollider_OBB::Render()
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

void CCollider_OBB::Collision_Enter(CCollider* pCollider)
{
	__super::Collision_Enter(pCollider);
}

void CCollider_OBB::Collision_Continue(CCollider* pCollider)
{
	__super::Collision_Continue(pCollider);
}

void CCollider_OBB::Collision_Exit(CCollider* pCollider)
{
	__super::Collision_Exit(pCollider);
}


CCollider_OBB* CCollider_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCollider_OBB* pInstance = new CCollider_OBB(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Box Create Failed.");
		return nullptr;
	}
	return pInstance;
}

CComponent* CCollider_OBB::Clone(void* pArg)
{
	CCollider_OBB* pInstance = new CCollider_OBB(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Box Clone Failed.");
		return nullptr;
	}
	return pInstance;
}

void CCollider_OBB::Free()
{
		
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pNode);
	Safe_Release(m_pOwnerTransformCom);

	__super::Free();
}

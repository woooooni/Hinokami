#include "RigidBody.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "GameObject.h"

_uint CCollider::g_iNextID = 0;

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER_TYPE eColliderType)
	:CComponent(pDevice, pContext)
	, m_eColliderType(eColliderType)
{
	
}

CCollider::CCollider(CCollider& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_pBatch(rhs.m_pBatch)
	, m_eColliderType(rhs.m_eColliderType)
	, m_iColliderID(g_iNextID++)
{

}

HRESULT CCollider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCodes = nullptr;
	size_t			iLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCodes, iLength, &m_pInputLayout)))
		return E_FAIL;

	COLLIDER_DESC* pDesc = (COLLIDER_DESC*)pArg;

	m_pNode = pDesc->pNode;
	Safe_AddRef(m_pNode);

	m_pOwnerTransformCom = pDesc->pOwnerTransform;
	Safe_AddRef(m_pOwnerTransformCom);

	m_vOffsetPosition = pDesc->vOffsetPosition;
	m_ModelPivotMatrix = pDesc->ModePivotMatrix;

	return S_OK;
}

void CCollider::LateTick_Collider(_float fTimeDelta)
{

	_matrix FinalMatrix;
	_matrix OffsetMatrix = XMMatrixIdentity(); 
	OffsetMatrix.r[CTransform::STATE_POSITION] = XMVectorSetW(XMLoadFloat3(&m_vOffsetPosition), 1.f);
	

	if (nullptr == m_pNode)
		FinalMatrix = OffsetMatrix * XMLoadFloat4x4(&m_ModelPivotMatrix) * m_pOwnerTransformCom->Get_WorldMatrix();
	else
	{
		FinalMatrix = OffsetMatrix * m_pNode->Get_CombinedTransformation() * XMLoadFloat4x4(&m_ModelPivotMatrix) * m_pOwnerTransformCom->Get_WorldMatrix();
	}
		

	XMStoreFloat4x4(&m_FinalMatrix, FinalMatrix);
}



void CCollider::Collision_Enter(CCollider* pCollider)
{
	m_vColor = _float4(1.f, 0.f, 0.f, 1.f);

	COLLISION_INFO tCollisionInfo;
	ZeroMemory(&tCollisionInfo, sizeof(COLLISION_INFO));

	tCollisionInfo.pMyCollider = this;
	tCollisionInfo.pOther = pCollider->Get_Owner();
	tCollisionInfo.pOtherCollider = pCollider;

	m_pOwner->Collision_Enter(tCollisionInfo);
}

void CCollider::Collision_Continue(CCollider* pCollider)
{
	COLLISION_INFO tCollisionInfo;
	ZeroMemory(&tCollisionInfo, sizeof(COLLISION_INFO));

	tCollisionInfo.pMyCollider = this;
	tCollisionInfo.pOther = pCollider->Get_Owner();
	tCollisionInfo.pOtherCollider = pCollider;
	

	m_pOwner->Collision_Continue(tCollisionInfo);
}

void CCollider::Collision_Exit(CCollider* pCollider)
{
	COLLISION_INFO tCollisionInfo;
	ZeroMemory(&tCollisionInfo, sizeof(COLLISION_INFO));

	tCollisionInfo.pMyCollider = this;
	tCollisionInfo.pOther = pCollider->Get_Owner();
	tCollisionInfo.pOtherCollider = pCollider;

	m_vColor = _float4(0.f, 1.f, 0.f, 1.f);
	m_pOwner->Collision_Exit(tCollisionInfo);
	
}

CComponent* CCollider::Clone(void* pArg)
{
	return nullptr;
}

void CCollider::Free()
{
		
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pNode);
	Safe_Release(m_pOwnerTransformCom);

	__super::Free();
}

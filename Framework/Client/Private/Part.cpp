#include "stdafx.h"
#include "..\Public\Part.h"

#include "GameInstance.h"
#include "HierarchyNode.h"

CPart::CPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObejctTag, _uint iObjectType)
	: CGameObject(pDevice, pContext, strObejctTag, iObjectType)
{

}

CPart::CPart(const CPart& rhs)
	: CGameObject(rhs)
{

}

CHierarchyNode* CPart::Get_Socket(const wstring& strNodeName)
{
	if (nullptr == m_pModelCom)
		return nullptr;

	return m_pModelCom->Get_HierarchyNode(strNodeName);
}


_float4x4 CPart::Get_SocketPivotMatrix()
{
	if (nullptr == m_pModelCom)
		return _float4x4();

	_float4x4 PivotMatrix;
	XMStoreFloat4x4(&PivotMatrix, m_pModelCom->Get_PivotMatrix());
	return PivotMatrix;
}

void CPart::Set_SocketBone(CHierarchyNode* pNode)
{
	Safe_Release(m_pSocketBone);
	m_pSocketBone = pNode;
	Safe_AddRef(m_pSocketBone);
}


HRESULT CPart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPart::Initialize(void* pArg)
{
	__super::Initialize(nullptr);
	if (nullptr != pArg)
	{
		PART_DESC* pPartDesc = (PART_DESC*)pArg;
		m_pParentTransform = pPartDesc->pParentTransform;
		m_pOwner = pPartDesc->pOwner;

		m_pSocketBone = pPartDesc->pSocketBone;
		Safe_AddRef(m_pSocketBone);

		m_SocketPivotMatrix = pPartDesc->SocketPivot;
		Safe_AddRef(m_pOwner);
		Safe_AddRef(m_pParentTransform);
	}

	return S_OK;
}

void CPart::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
	for (_uint i = 0; i < CCollider::DETECTION_TYPE::DETECTION_END; ++i)
	{
		for (auto& pCollider : m_Colliders[i])
			m_pRendererCom->Add_Debug(pCollider);
	}
}

HRESULT CPart::Render()
{
	return __super::Render();
}


HRESULT CPart::Compute_RenderMatrix(_fmatrix ChildMatrix)
{
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_OriginRotationTransform) * ChildMatrix * m_pParentTransform->Get_WorldMatrix());
	return S_OK;
}


CGameObject* CPart::Clone(void* pArg)
{
	return nullptr;
}

void CPart::Free()
{
	__super::Free();

	Safe_Release(m_pParentTransform);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pSocketBone);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_AddRef(m_pOwner);
}

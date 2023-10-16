#include "stdafx.h"
#include "..\Public\Part.h"

#include "GameInstance.h"

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


HRESULT CPart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPart::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		PART_DESC* pPartDesc = (PART_DESC*)pArg;
		m_pParentTransform = pPartDesc->pParentTransform;
		Safe_AddRef(m_pParentTransform);
	}

	return S_OK;
}


HRESULT CPart::Compute_RenderMatrix(_fmatrix ChildMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, ChildMatrix * m_pParentTransform->Get_WorldMatrix());
	return S_OK;
}


CPart* CPart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
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
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

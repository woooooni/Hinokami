#include "..\Public\HierarchyNode.h"
#include "Utils.h"
#include "Model.h"

CHierarchyNode::CHierarchyNode()
{

}

CHierarchyNode::CHierarchyNode(const CHierarchyNode& rhs)
	: m_strName(rhs.m_strName)
	, m_strParentName(rhs.m_strParentName)
	, m_OriginTransformation(rhs.m_OriginTransformation)
	, m_iDepth(rhs.m_iDepth)
	, m_OffsetMatrix(rhs.m_OffsetMatrix)
{
	XMStoreFloat4x4(&m_CombinedTransformation, XMMatrixIdentity());
	m_Transformation = m_OriginTransformation;
}

HRESULT CHierarchyNode::Initialize(aiNode* pAINode, CHierarchyNode* pParent, _uint iDepth)
{
	/* �� �̸� ����. */
	m_strName = CUtils::ToWString(pAINode->mName.C_Str());

	if (pParent != nullptr)
		m_strParentName = pParent->Get_Name();
	else
		m_strParentName = L"";

	/* ����ü�� ���� ��������� �޾ƿö��� �ݵ�� ��ġ�ؼ� �޾ƿͶ�. */
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());


	memcpy(&m_Transformation, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&m_Transformation, XMMatrixTranspose(XMLoadFloat4x4(&m_Transformation)));

	m_OriginTransformation = m_Transformation;

	m_iDepth = iDepth;
	m_pParent = pParent;

	XMStoreFloat4x4(&m_CombinedTransformation, XMMatrixIdentity());

	Safe_AddRef(m_pParent);

	return S_OK;
}

HRESULT CHierarchyNode::Initialize_Bin(CModel* pModel)
{
	m_OriginTransformation = m_Transformation;
	m_pParent = pModel->Get_HierarchyNode(m_strParentName);
	
	if (m_pParent != nullptr)
	{
		m_strParentName = m_pParent->Get_Name();
		Safe_AddRef(m_pParent);
	}
	else
	{
		m_strParentName = L"";
	}


	
	return S_OK;
}


void CHierarchyNode::Set_CombinedTransformation()
{
	if (nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformation, XMLoadFloat4x4(&m_Transformation) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformation));
	else
		m_CombinedTransformation = m_Transformation;
}

void CHierarchyNode::Set_OffsetMatrix(_fmatrix OffsetMatrix)
{
	XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix);
}

CHierarchyNode* CHierarchyNode::Create(aiNode* pAINode, CHierarchyNode* pParent, _uint iDepth)
{
	CHierarchyNode* pInstance = new CHierarchyNode();

	if (FAILED(pInstance->Initialize(pAINode, pParent, iDepth)))
	{
		MSG_BOX("Failed To Created : CHierarchyNode");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CHierarchyNode* CHierarchyNode::Create_Bin()
{
	CHierarchyNode* pInstance = new CHierarchyNode();
	return pInstance;
}

CHierarchyNode* CHierarchyNode::Clone()
{
	CHierarchyNode* pInstance = new CHierarchyNode(*this);

	return pInstance;
}


void CHierarchyNode::Free()
{
	Safe_Release(m_pParent);
}

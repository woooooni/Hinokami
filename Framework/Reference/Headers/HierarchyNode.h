#pragma once

#include "Base.h"

/* 계층구조(뼈끼리의 상속관계)를 표현하기위한 aiNode이다. */

BEGIN(Engine)

class ENGINE_DLL CHierarchyNode final : public CBase
{
private:
	CHierarchyNode();
	CHierarchyNode(const CHierarchyNode& rhs);
	virtual ~CHierarchyNode() = default;

public:
	const wstring& Get_Name() const {
		return m_strName;
	}

	const wstring Get_ParentName() const {
		if (m_pParent == nullptr)
			return wstring();

		return m_pParent->Get_Name();
	}

	_uint Get_Depth() const {
		return m_iDepth;
	}

	_matrix Get_OffSetMatrix() {
		return XMLoadFloat4x4(&m_OffsetMatrix);
	}

	_matrix Get_CombinedTransformation() {
		return XMLoadFloat4x4(&m_CombinedTransformation);
	}

	_matrix Get_RootCombinedTransformation(){
		return XMLoadFloat4x4(&m_RootAnim_CombinedTransformation);
	}


public:
	void Set_Transformation(_fmatrix Transformation) {
		XMStoreFloat4x4(&m_Transformation, Transformation);
	}
	_matrix Get_TransformationMatrix() { return XMLoadFloat4x4(&m_Transformation); }

public:
	HRESULT Initialize(aiNode * pAINode, class CHierarchyNode* pParent, _uint iDepth);
	HRESULT Initialize_Bin(class CModel* pModel);

public:
	void Set_CombinedTransformation(const wstring& strRootBoneName);
	void Set_OffsetMatrix(_fmatrix OffsetMatrix);

private:
	wstring				m_strName;
	wstring				m_strParentName;

	_float4x4			m_OffsetMatrix;
	_float4x4			m_Transformation;
	_float4x4			m_OriginTransformation;
	_float4x4			m_CombinedTransformation;
	_float4x4			m_RootAnim_CombinedTransformation;

	CHierarchyNode*		m_pParent = nullptr;
	_uint				m_iDepth = 0;

public:
	static CHierarchyNode* Create(aiNode * pAINode, class CHierarchyNode* pParent, _uint iDepth);
	static CHierarchyNode* Create_Bin();
	CHierarchyNode* Clone();
	virtual void Free();

	friend class CModel_Manager;
};

END
#pragma once


#include "VIBuffer.h"
#include "Model.h"


/* 모델을 구성하는 하나의 메시. */
/* 이 메시를 구성하는 정점, 인덱스 버퍼를 보관한다. */

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);

	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Initialize_Bin(CModel* pModel, const vector<wstring>& BoneNames);

public:
	const vector<class CHierarchyNode*>& Get_Bones() { return m_Bones; }

public:
	HRESULT SetUp_HierarchyNodes(class CModel* pModel, aiMesh* pAIMesh);
	HRESULT SetUp_HierarchyNodes(class CModel* pModel);
	void SetUp_BoneMatrices(ID3D11Texture2D* pTexture, vector<_float4x4>& Matrices, _fmatrix PivotMatrix);


public:
	_uint Get_BoneCount() { return m_iNumBones; }

private:
	wstring				m_strName;
	_uint				m_iMaterialIndex = 0;
	_bool				m_bFromBinary = false;
private:
	/* 이 메시에 영향ㅇ르 주는 뼈의 갯수. */
	_uint							m_iNumBones = 0;

	/* 이 메시에 영향을 주는 뼈들을 모아놓느다. */
	/* why ? : 메시 별로 렌더링할 때 이 메시에 영향ㅇ르 주는 르의 행렬을 모아서 셰{ㅇ디러ㅗ 도ㅓㄴ질려고. */
	vector<class CHierarchyNode*>	m_Bones;
	vector<wstring>		m_BoneNames;


	vector<VTXANIMMODEL> m_AnimVertices;
	vector<VTXMODEL> m_NonAnimVertices;

private:
	HRESULT Ready_Vertices(const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Ready_AnimVertices(const aiMesh* pAIMesh, CModel* pModel);

	HRESULT Ready_Bin_Vertices();
	HRESULT Ready_Bin_AnimVertices();

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	static CMesh* Create_Bin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	friend class CModel_Manager;

};

END
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

public:
	const vector<class CHierarchyNode*>& Get_Bones() { return m_Bones; }

public:
	HRESULT SetUp_HierarchyNodes(class CModel* pModel, aiMesh* pAIMesh);
	void SetUp_BoneMatrices(ID3D11Texture1D* pTexture, vector<_float4x4>& Matrices, _fmatrix PivotMatrix);

	

private:
	wstring				m_strName;
	_uint				m_iMaterialIndex = 0;
private:
	/* 이 메시에 영향ㅇ르 주는 뼈의 갯수. */
	_uint							m_iNumBones = 0;

	/* 이 메시에 영향을 주는 뼈들을 모아놓느다. */
	/* why ? : 메시 별로 렌더링할 때 이 메시에 영향ㅇ르 주는 뼏르의 행렬을 모아서 셰{ㅇ디러ㅗ 도ㅓㄴ질려고. */
	vector<class CHierarchyNode*>	m_Bones;


private:
	HRESULT Ready_Vertices(const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Ready_AnimVertices(const aiMesh* pAIMesh, CModel* pModel);

	HRESULT Ready_Vertices(const vector<VTXMODEL>& Vertices, const vector<FACEINDICES32>& Indices);
	HRESULT Ready_AnimVertices(const vector<VTXANIMMODEL>& Vertices, const vector<FACEINDICES32>& Indices);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	static CMesh* Create_Bin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	friend class CData_Manager;

};

END
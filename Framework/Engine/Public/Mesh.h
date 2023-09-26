#pragma once


#include "VIBuffer.h"
#include "Model.h"


/* ���� �����ϴ� �ϳ��� �޽�. */
/* �� �޽ø� �����ϴ� ����, �ε��� ���۸� �����Ѵ�. */

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
	/* �� �޽ÿ� ���⤷�� �ִ� ���� ����. */
	_uint							m_iNumBones = 0;

	/* �� �޽ÿ� ������ �ִ� ������ ��Ƴ�����. */
	/* why ? : �޽� ���� �������� �� �� �޽ÿ� ���⤷�� �ִ� ������ ����� ��Ƽ� ��{���𷯤� ���ä�������. */
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
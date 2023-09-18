#pragma once


#include "VIBuffer.h"
#include "Model.h"
#include "AsFileUtils.h"

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
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT	LoadData_FromMeshFile(CModel::TYPE eModelType, CAsFileUtils* pFileUtils, _fmatrix PivotMatrix);
	HRESULT	LoadData_FromConverter(CModel::TYPE eModelType, shared_ptr<asMesh> pMesh, _fmatrix PivotMatrix);


private:
	//char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = 0;
private:
	/* �� �޽ÿ� ���⤷�� �ִ� ���� ����. */
	_uint							m_iNumBones = 0;

	/* �� �޽ÿ� ������ �ִ� ������ ��Ƴ�����. */
	/* why ? : �޽� ���� �������� �� �� �޽ÿ� ���⤷�� �ִ� ������ ����� ��Ƽ� ��{���𷯤� ���ä�������. */
	vector<class CHierarchyNode*>	m_Bones;


	_uint							m_iBoneIndex;
	wstring							m_szName;
	wstring							m_szMaterialName;
private:
	HRESULT Ready_Vertices(VTXANIMMODEL* pVertices, _fmatrix PivotMatrix);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
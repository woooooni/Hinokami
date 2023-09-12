#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	class CHierarchyNode* Get_HierarchyNode(const char* pNodeName);

	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_uint Get_MaterialIndex(_uint iMeshIndex);

	void Set_AnimIndex(_uint iAnimIndex) {
		m_iCurrentAnimIndex = iAnimIndex;
	}
	_uint Get_CurAnimationIndex() { return m_iCurrentAnimIndex; }

	_uint Get_MaxAnimIndex();

	_matrix Get_PivotMatrix() {
		return XMLoadFloat4x4(&m_PivotMatrix);
	}

	
public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const wstring& strModelFilePath, const wstring& strModelFileName, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);


public:
	HRESULT SetUp_OnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const wstring& strConstantName);
	/* �ִϸ��̼��� ����Ѵ�. */
	/* 1. �ش� �ִϸ��̼ǿ��� ����ϴ� ��� ������  Transformation ����� �����Ѵ�. */
	/* 2. Transformation�� �ֻ��� �θ�κ��� �ڽ����� ��� �������Ѱ���.(CombinedTransformation) */
	/* 3. �ִϸ��̼ǿ� ���� ������ ������ CombinedTransfromation�� �����Ѵ�. */
	HRESULT Play_Animation(_float fTimeDelta);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);

private:
	const aiScene*				m_pAIScene = nullptr;
	Assimp::Importer			m_Importer;

//private:
//	const DATA_SCENE* m_pDataScene = nullptr;
//	_bool m_bBinData = false;
	
private:
	_float4x4					m_PivotMatrix;
	TYPE						m_eModelType = TYPE_END;

private:
	_uint							m_iNumMeshes = 0;
	vector<class CMesh*>			m_Meshes;
	typedef vector<class CMesh*>	MESHES;

private:
	_uint									m_iNumMaterials = 0;
	vector<MATERIALDESC>					m_Materials;

private:
	vector<class CHierarchyNode*>			m_HierarchyNodes;

private:
	_uint								m_iCurrentAnimIndex = 0;
	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>			m_Animations;
	vector<_float4x4>	m_Matrices;
	

private:
	ID3D11Texture1D* m_pMatixTexture = nullptr;
	ID3D11ShaderResourceView* m_pMatrixSRV = nullptr;


private:
	HRESULT Ready_MeshContainers(_fmatrix PivotMatrix);
	HRESULT Ready_Materials(const wstring& strModelFilePath);
	HRESULT Ready_HierarchyNodes(aiNode* pNode, class CHierarchyNode* pParent, _uint iDepth);
	HRESULT Ready_Animations();
	HRESULT Ready_VTF_Texture();


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const wstring& strModelFilePath, const wstring& strModelFileName, _fmatrix PivotMatrix = XMMatrixIdentity());
	
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
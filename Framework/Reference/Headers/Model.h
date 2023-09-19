#pragma once

#include "Component.h"
#include "AsTypes.h"

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

	_uint Get_CurrAnimationIndex() { return m_iCurrentAnimIndex; }

	_uint Get_MaxAnimIndex();

	_matrix Get_PivotMatrix() {
		return XMLoadFloat4x4(&m_PivotMatrix);
	}

	TYPE Get_ModelType() {
		return m_eModelType;
	}

	
	vector<class CAnimation*>& Get_Animations() { return m_Animations; }

public:
	void Swap_Animation(_uint iSrc, _uint iDest)
	{
		if (iDest >= m_Animations.size())
			iDest = m_Animations.size() - 1;
		if (iDest < 0)
			iDest = 0;

		CAnimation* Temp = m_Animations[iSrc];
		m_Animations[iSrc] = m_Animations[iDest];
		m_Animations[iDest] = Temp;

		m_iCurrentAnimIndex = iDest;
	}

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const wstring& strModelFilePath, const wstring& strModelFileName, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);


public:
	HRESULT SetUp_OnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const wstring& strConstantName);
	HRESULT SetUpAnimation_OnShader(class CShader* pShader);
	/* �ִϸ��̼��� ����Ѵ�. */
	/* 1. �ش� �ִϸ��̼ǿ��� ����ϴ� ��� ������  Transformation ����� �����Ѵ�. */
	/* 2. Transformation�� �ֻ��� �θ�κ��� �ڽ����� ��� �������Ѱ���.(CombinedTransformation) */
	/* 3. �ִϸ��̼ǿ� ���� ������ ������ CombinedTransfromation�� �����Ѵ�. */
	HRESULT Play_Animation(_float fTimeDelta);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);

	HRESULT Load_AssetFile_FromFBX();
	HRESULT Load_AssetFile_FromBinary();
	HRESULT Export_AssetData();


	HRESULT Delete_ModelAnimation(_uint iIndex);

private:
	HRESULT Load_ModelData_FromFile(_fmatrix PivotMatrix);
	HRESULT Load_MaterialData_FromFile();
	HRESULT Load_AnimationData_FromFile(_fmatrix PivotMatrix);
	HRESULT Create_AnimationTexture(_fmatrix PivotMatrix);


	HRESULT Load_ModelData_FromConverter(_fmatrix PivotMatrix);
	HRESULT Load_MaterialData_FromConverter();
	HRESULT Load_AnimationData_FromConverter(_fmatrix PivotMatrix);

private:
	const aiScene*				m_pAIScene = nullptr;
	Assimp::Importer			m_Importer;

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
	_uint								m_iNextAnimIndex = 0;
	uint64								m_iNumAnimations = 0;
	vector<class CAnimation*>			m_Animations;


	//Model Load
	wstring								m_strFilePath;
	wstring								m_strFileName;


	vector<shared_ptr<ModelBone>>		m_ModelBones;
	shared_ptr<ModelBone>				m_RootBone;

	class CModelConverter*					m_pConverter = nullptr;
private:
	HRESULT Ready_MeshContainers(_fmatrix PivotMatrix);
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_HierarchyNodes(aiNode* pNode, class CHierarchyNode* pParent, _uint iDepth);
	HRESULT Ready_Animations();


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const wstring& strModelFilePath, const wstring& strModelFileName, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
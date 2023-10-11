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
	class CHierarchyNode* Get_HierarchyNode(const wstring & strNodeName);

	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_uint Get_MaterialIndex(_uint iMeshIndex);
	class CTexture* Get_MaterialTexture(_uint iMeshIndex, _uint iTextureType);

	HRESULT Set_Animation(const wstring & strAnimationName);
	void Set_AnimIndex(_uint iAnimIndex);

	void Complete_Interpolation();

	_matrix Get_PivotMatrix() {
		return XMLoadFloat4x4(&m_PivotMatrix);
	}

	TYPE Get_ModelType() { return m_eModelType; }


public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const wstring & strModelFilePath, const wstring & strModelFileName, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Initialize_Bin(void* pArg);

public:
	HRESULT SetUp_OnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pConstantName);
	/* �ִϸ��̼��� ����Ѵ�. */
	/* 1. �ش� �ִϸ��̼ǿ��� ����ϴ� ��� ������  Transformation ����� �����Ѵ�. */
	/* 2. Transformation�� �ֻ��� �θ�κ��� �ڽ����� ��� �������Ѱ���.(CombinedTransformation) */
	/* 3. �ִϸ��̼ǿ� ���� ������ ������ CombinedTransfromation�� �����Ѵ�. */
	HRESULT Play_Animation(_float fTimeDelta);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);


	HRESULT Swap_Animation(_uint iSrcIndex, _uint iDestIndex);
	HRESULT Delete_Animation(_uint iIndex);
	_bool Is_InterpolatingAnimation() { return m_bInterpolationAnimation; }
public:
	vector<class CAnimation*>& Get_Animations() { return m_Animations; }
	_uint Get_CurrAnimationIndex() { return m_iCurrentAnimIndex; }



public:
	const aiScene* Get_Scene() { return m_pAIScene; }

private:
	wstring m_strName;
	wstring m_strFileName;
	wstring m_strFolderPath;

private:
	const aiScene* m_pAIScene = nullptr;

	Assimp::Importer			m_Importer;

	_float4x4					m_PivotMatrix;
	TYPE						m_eModelType = TYPE_END;
	_bool						m_bFromBinary = false;
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
	_int								m_iNextAnimIndex = -1;
	_bool								m_bInterpolationAnimation = false;

	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>			m_Animations;


private:
	ID3D11Texture1D* m_pMatrixTexture = nullptr;
	ID3D11ShaderResourceView* m_pSRV;
	vector<_float4x4> m_Matrices;


private:
	HRESULT Ready_MeshContainers(_fmatrix PivotMatrix);
	HRESULT Ready_Materials(const wstring & pModelFilePath);
	HRESULT Ready_HierarchyNodes(aiNode * pNode, class CHierarchyNode* pParent, _uint iDepth);
	HRESULT Ready_Animations();
	HRESULT Ready_Animation_Texture();

private:
	static CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const wstring & strModelFilePath, const wstring & strModelFileName, _fmatrix PivotMatrix = XMMatrixIdentity());
	static CModel* Create_Bin(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const wstring & strModelFilePath, const wstring & strModelFileName, _fmatrix PivotMatrix = XMMatrixIdentity());

public:
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;

	friend class CModel_Manager;
};



END
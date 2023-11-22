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
	const vector<class CMesh*>& Get_Meshes() { return m_Meshes; }

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
	/* 애니메이션을 재생한다. */
	/* 1. 해당 애니메이션에서 사용하는 모든 뼈들의  Transformation 행렬을 갱신한다. */
	/* 2. Transformation를 최상위 부모로부터 자식으로 계속 누적시켜간다.(CombinedTransformation) */
	/* 3. 애니메이션에 의해 움직인 뼈들의 CombinedTransfromation을 셋팅한다. */
	HRESULT Play_Animation(class CTransform* pTransform, _float fTimeDelta);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);
	HRESULT Render_Instancing(class CShader* pShader, _uint iMeshIndex, class CVIBuffer_Instancing* pInstancingBuffer, const vector<_float4x4>& WorldMatrices, _uint iPassIndex = 0);


	HRESULT Swap_Animation(_uint iSrcIndex, _uint iDestIndex);
	HRESULT Delete_Animation(_uint iIndex);
	_bool Is_InterpolatingAnimation() { return m_bInterpolationAnimation; }

	_bool Is_Animation_Finished(_uint iAnimationIndex);

	

public:
	vector<class CAnimation*>& Get_Animations() { return m_Animations; }
	_int Find_AnimationIndex(const wstring& strAnimationTag);
	_uint Get_CurrAnimationIndex() { return m_iCurrentAnimIndex; }

	class CAnimation* Get_CurrAnimation() { return m_Animations[m_iCurrentAnimIndex]; }



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
	CHierarchyNode* m_pRootNode;
	vector<class CHierarchyNode*>			m_HierarchyNodes;

private:
	_uint								m_iCurrentAnimIndex = 0;
	_int								m_iNextAnimIndex = -1;
	_bool								m_bInterpolationAnimation = false;
	_bool								m_bFirstRootConvert = false;
	_float4								m_vPreAnimPos = {0.f, 0.f, 0.f, 1.f };

	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>			m_Animations;


private:
	ID3D11Texture2D* m_pMatrixTexture = nullptr;
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
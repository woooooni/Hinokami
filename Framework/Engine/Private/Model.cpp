#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Shader.h"
#include "Utils.h"
#include "Transform.h"
#include <fstream>
#include <filesystem>
#include "VIBuffer_Instancing.h"
#include "GameObject.h"
#include "Navigation.h"


CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
	

}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Meshes(rhs.m_Meshes)
	, m_Materials(rhs.m_Materials)
	, m_eModelType(rhs.m_eModelType)
	/*, m_HierarchyNodes(rhs.m_HierarchyNodes)*/
	, m_Animations(rhs.m_Animations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_pMatrixTexture(rhs.m_pMatrixTexture)
	, m_Matrices(rhs.m_Matrices)
	, m_pSRV(rhs.m_pSRV)
	, m_strName(rhs.m_strName)
	, m_strFileName(rhs.m_strFileName)
	, m_strFolderPath(rhs.m_strFolderPath)
	, m_bFromBinary(rhs.m_bFromBinary)
	
{
	for (auto& pMeshContainer : m_Meshes)
		Safe_AddRef(pMeshContainer);


	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}

	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);

	for (auto& pNode : rhs.m_HierarchyNodes)
	{
		CHierarchyNode* pNewNode = pNode->Clone();
		m_HierarchyNodes.push_back(pNewNode);

		if (pNode->Get_Name() == L"Root")
			m_pRootNode = pNewNode;
		

		Safe_AddRef(pNode);
	}

	for (auto& pNode : m_HierarchyNodes)	
		pNode->Initialize_Bin(this);

		

	Safe_AddRef(m_pSRV);
	Safe_AddRef(m_pMatrixTexture);
}


HRESULT CModel::Initialize_Prototype(TYPE eType, const wstring& strModelFolderPath, const wstring& strModelFileName, _fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	_tchar		szFullPath[MAX_PATH] = L"";

	m_strFolderPath = strModelFolderPath;
	m_strFileName = strModelFileName;

	lstrcpy(szFullPath, m_strFolderPath.c_str());
	lstrcat(szFullPath, strModelFileName.c_str());

	_uint		iFlag = 0;

	m_eModelType = eType;

	// aiProcess_PreTransformVertices : 모델을 구성하는 메시 중, 이 메시의 이름과 뼈의 이름이 같은 상황이라면 이 뼈의 행렬을 메시의 정점에 다 곱해서 로드한다. 
	// 모든 애니메이션 정보는 폐기된다. 
	if (TYPE_NONANIM == eType)
		iFlag |= aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
	else
		iFlag |= aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;

	/* 파일의 정보를 읽어서 aiScene안에 보관한다.  */

	m_pAIScene = m_Importer.ReadFile(CUtils::ToString(szFullPath).c_str(), iFlag);

	if (nullptr == m_pAIScene)
		return E_FAIL;

	if (FAILED(Ready_HierarchyNodes(m_pAIScene->mRootNode, nullptr, 0)))
		return E_FAIL;

	/* 모델을 구성하는 메시들을 만든다. */
	/* 모델은 여러개의 메시로 구성되어있다. */
	/* 각 메시의 정점들과 인덱스들을 구성한다. */
	if (FAILED(Ready_MeshContainers(PivotMatrix)))
		return E_FAIL;

	/* 머테리얼정보다.(빛을 받았을때 리턴해야할 색상정보.) */
	/* 모델마다정의?, 정점마다정의? 픽셀마다 정의(o) 텍스쳐로 표현된다. */
	if (FAILED(Ready_Materials(m_strFolderPath)))
		return E_FAIL;


	/* 애니메이션의 정보를 읽어서 저장한다.  */
	/* 애니메이션 정보 : 애니메이션이 재생되는데 걸리는 총 시간(Duration),  애니메이션의 재생속도( mTickPerSecond), 몇개의 채널(mNumChannels) 에 영향르 주는가. 각채널의 정보(aiNodeAnim)(mChannels) */
	/* mChannel(aiNodeAnim, 애니메이션이 움직이는 뼈) 에 대한 정보를 구성하여 객체화한다.(CChannel) */
	/* 채널 : 뼈. 이 뼈는 한 애니메이션 안에서 사용된다. 그 애니메이션 안에서 어떤 시간, 시간, 시간, 시간대에 어떤 상태를 표현하면 되는지에 대한 정보(keyframe)들을 다므낟. */
	/* keyframe : 어떤시간?, 상태(vScale, vRotation, vPosition) */
	if (FAILED(Ready_Animations()))
		return E_FAIL;

	if (FAILED(Ready_Animation_Texture()))
		return E_FAIL;



	return S_OK;
}


HRESULT CModel::Initialize(void* pArg)
{
	_uint		iNumMeshes = 0;

	vector<CMesh*>		MeshContainers;

	for (auto& pPrototype : m_Meshes)
	{
		CMesh* pMeshContainer = (CMesh*)pPrototype->Clone();
		if (nullptr == pMeshContainer)
			return E_FAIL;

		MeshContainers.push_back(pMeshContainer);

		Safe_Release(pPrototype);
	}

	m_Meshes.clear();

	m_Meshes = MeshContainers;

	if (TYPE_ANIM == m_eModelType)
	{
		for (auto& pMeshContainer : m_Meshes)
		{
			if (nullptr != pMeshContainer)
				pMeshContainer->SetUp_HierarchyNodes(this, m_pAIScene->mMeshes[iNumMeshes++]);
		}
	}

	vector<CAnimation*>		Animations;

	for (auto& pPrototype : m_Animations)
	{
		CAnimation* pAnimation = pPrototype->Clone(this);
		if (nullptr == pAnimation)
			return E_FAIL;

		Animations.push_back(pAnimation);

		Safe_Release(pPrototype);
	}

	m_Animations.clear();

	m_Animations = Animations;


	m_Matrices.reserve(m_HierarchyNodes.size());
	return S_OK;
}

HRESULT CModel::Initialize_Bin(void* pArg)
{
	if (TYPE_ANIM == m_eModelType)
	{
		_uint		iNumMeshes = 0;

		vector<CMesh*>		MeshContainers;
		MeshContainers.reserve(m_Meshes.size());

		for (auto& pPrototype : m_Meshes)
		{
			CMesh* pMeshContainer = (CMesh*)pPrototype->Clone();
			if (nullptr == pMeshContainer)
				return E_FAIL;

			MeshContainers.push_back(pMeshContainer);

			Safe_Release(pPrototype);
		}

		m_Meshes.clear();

		m_Meshes = MeshContainers;

		for (auto& pMeshContainer : m_Meshes)
		{
			if (nullptr != pMeshContainer)
				pMeshContainer->SetUp_HierarchyNodes(this);
		}
	}

	vector<CAnimation*>		Animations;
	Animations.reserve(m_Animations.size());

	for (auto& pPrototype : m_Animations)
	{
		CAnimation* pAnimation = pPrototype->Clone(this);
		if (nullptr == pAnimation)
			return E_FAIL;

		Animations.push_back(pAnimation);

		Safe_Release(pPrototype);
	}

	m_Animations.clear();

	m_Animations = Animations;

	m_Matrices.reserve(m_HierarchyNodes.size());
	return S_OK;
}

CHierarchyNode* CModel::Get_HierarchyNode(const wstring& strNodeName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)
		{
			return strNodeName == pNode->Get_Name();
		});

	if (iter == m_HierarchyNodes.end())
		return nullptr;

	return *iter;
}

_uint CModel::Get_MaterialIndex(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Get_MaterialIndex();
}

CTexture* CModel::Get_MaterialTexture(_uint iMeshIndex, _uint iTextureType)
{
	return m_Materials[iMeshIndex].pTexture[iTextureType];
}

HRESULT CModel::Set_Animation(const wstring& strAnimationName)
{
	for (size_t i = 0; i < m_Animations.size(); ++i)
	{
		if (strAnimationName == m_Animations[i]->Get_AnimationName())
		{
			Set_AnimIndex(i);
			return S_OK;
		}
	}
	return E_FAIL;
}

void CModel::Set_AnimIndex(_uint iAnimIndex)
{
	if (iAnimIndex >= m_Animations.size())
		iAnimIndex = 0;

	m_Animations[m_iCurrentAnimIndex]->Reset_Animation();
	m_iNextAnimIndex = iAnimIndex;
	m_bInterpolationAnimation = true;
	m_bFirstRootConvert = true;
}

void CModel::Complete_Interpolation()
{
	m_iCurrentAnimIndex = m_iNextAnimIndex;
	m_iNextAnimIndex = -1;
	m_bInterpolationAnimation = false;
}





HRESULT CModel::SetUp_OnShader(CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pConstantName)
{
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (m_Materials[iMaterialIndex].pTexture[eTextureType] == nullptr)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pTexture[eTextureType]->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CModel::Play_Animation(CTransform* pTransform, _float fTimeDelta)
{
	
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return E_FAIL;

	_vector vPrevAnimPos, vNextAnimPos;
	vPrevAnimPos = m_pRootNode->Get_RootCombinedTransformation().r[CTransform::STATE_POSITION];

	if (m_bInterpolationAnimation) 
	{
		m_Animations[m_iCurrentAnimIndex]->Play_Animation(this, pTransform, m_Animations[m_iNextAnimIndex], fTimeDelta);
	}
	else
	{
		m_Animations[m_iCurrentAnimIndex]->Play_Animation(pTransform, fTimeDelta);
	}
		

	for (auto& pHierarchyNode : m_HierarchyNodes)
		pHierarchyNode->Set_CombinedTransformation(m_pRootNode->Get_Name());

	if (m_bInterpolationAnimation)	
		return S_OK;
	

	if (m_Animations[m_iCurrentAnimIndex]->Is_RootAnimation())
	{
		vNextAnimPos = m_pRootNode->Get_RootCombinedTransformation().r[CTransform::STATE_POSITION];
		if (m_bFirstRootConvert)
		{
			vNextAnimPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
			vPrevAnimPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
			m_bFirstRootConvert = 0.f;
		}


		_vector vPos = pTransform->Get_State(CTransform::STATE_POSITION);
		_vector vDir = vPrevAnimPos - vNextAnimPos;
		_vector vWorldDir = XMVector3Normalize(XMVector3TransformNormal(vDir, pTransform->Get_WorldMatrix()));
		XMVectorSetY(vWorldDir, XMVectorGetY(vWorldDir) * -1.f);

		_float fDist = XMVectorGetX(XMVector3Length(vDir));

		vPos += vWorldDir * fDist * fTimeDelta;
		pTransform->Set_Position(vPos, fTimeDelta, m_pOwner->Get_Component<CNavigation>(L"Com_Navigation"));
	}
	

	return S_OK;
}

HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex, _uint iPassIndex)
{
	if (TYPE_ANIM == m_eModelType)
	{
		m_Meshes[iMeshIndex]->SetUp_BoneMatrices(m_pMatrixTexture, m_Matrices, XMLoadFloat4x4(&m_PivotMatrix));
		if (FAILED(pShader->Bind_Texture("g_MatrixPallete", m_pSRV)))
			return E_FAIL;
	}

	pShader->Begin(iPassIndex);
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Render_Instancing(CShader* pShader, _uint iMeshIndex, CVIBuffer_Instancing* pInstancingBuffer, const vector<_float4x4>& WorldMatrices, _uint iPassIndex)
{
	if (TYPE_ANIM == m_eModelType)
	{
		m_Meshes[iMeshIndex]->SetUp_BoneMatrices(m_pMatrixTexture, m_Matrices, XMLoadFloat4x4(&m_PivotMatrix));
		if (FAILED(pShader->Bind_Texture("g_MatrixPallete", m_pSRV)))
			return E_FAIL;
	}

	pShader->Begin(iPassIndex);
	pInstancingBuffer->Render(WorldMatrices, m_Meshes[iMeshIndex]);

	return S_OK;
}

HRESULT CModel::Swap_Animation(_uint iSrcIndex, _uint iDestIndex)
{

	if (TYPE_ANIM != m_eModelType)
		return E_FAIL;

	if (0 > iSrcIndex || 0 > iDestIndex || m_Animations.size() <= iSrcIndex || m_Animations.size() <= iDestIndex)
		return E_FAIL;

	CAnimation* Temp = m_Animations[iDestIndex];
	m_Animations[iDestIndex] = m_Animations[iSrcIndex];
	m_Animations[iSrcIndex] = Temp;

	m_iCurrentAnimIndex = iDestIndex;

	return S_OK;
}

HRESULT CModel::Delete_Animation(_uint iIndex)
{

	if (TYPE_ANIM != m_eModelType)
		return E_FAIL;

	if (0 > iIndex || m_Animations.size() <= iIndex)
		return E_FAIL;



	vector<CAnimation*>::iterator iter = m_Animations.begin();
	iter += iIndex;
	Safe_Release(*iter);

	m_Animations.erase(iter);

	m_iCurrentAnimIndex = 0 > m_iCurrentAnimIndex - 1 ? 0 : m_iCurrentAnimIndex;
	m_iCurrentAnimIndex = m_Animations.size() >= m_iCurrentAnimIndex ? m_Animations.size() - 1 : m_iCurrentAnimIndex;

	return S_OK;
}

_bool CModel::Is_Animation_Finished(_uint iAnimationIndex)
{
	if (iAnimationIndex >= m_Animations.size())
	{
		MSG_BOX("Animation Index Over.");
		return false;
	}

	return m_Animations[iAnimationIndex]->Is_Finished();
}


_int CModel::Find_AnimationIndex(const wstring& strAnimationTag)
{
	_int iIndex = 0;

	for (auto& pAnimation : m_Animations)
	{
		if (pAnimation->Get_AnimationName() == strAnimationTag)		
			return iIndex;

		++iIndex;
	}

	

	return -1;
}


HRESULT CModel::Ready_MeshContainers(_fmatrix PivotMatrix)
{
	/* 메시의 갯수를 얻어온다. */
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMeshContainer = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], this, PivotMatrix);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const wstring& ModelFilePath)
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	/* 이 모델은 몇개의 머테리얼 정보를 이용하는가. */
	/* 머테리얼(MATERIALDESC) : 텍스쳐[디퓨즈or앰비언트or노말or이미시브 등등등 ] */
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MATERIALDESC		MaterialDesc;
		ZeroMemory(&MaterialDesc, sizeof(MATERIALDESC));

		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		/* AI_TEXTURE_TYPE_MAX:디퓨즈or앰비언트or노말or이미시브 등등등 */
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString		strPath;

			/* 해당 재질을 표현하기위한 텍스쳐의 경로를 strPath에 받아온다. */
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
				continue;

			_tchar			szFullPath[MAX_PATH] = L"";
			_tchar			szFileName[MAX_PATH] = L"";
			_tchar			szExt[MAX_PATH] = L"";

			_wsplitpath_s(CUtils::ToWString(strPath.C_Str()).c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);


			lstrcpy(szFullPath, ModelFilePath.c_str());
			lstrcat(szFullPath, szFileName);
			lstrcat(szFullPath, szExt);

			memcpy(MaterialDesc.strName, &szFileName, sizeof(wchar_t) * MAX_PATH);

			MaterialDesc.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath);
			if (nullptr == MaterialDesc.pTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(MaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Ready_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth)
{
	/* pParent? : 부모 노드 주소. CombinedTransformation으로 그린다.
	CombinedTransformation놈을 만들려면 나의 Transformation * 부모의CombinedTranfsormation로 만든다. */
	/* 그래서 부모가 필요해. */
	/* iDepth? : Ready_HierarchyNodes함수를 재귀형태로 부르고ㅓ있기ㄸ매ㅜㄴ에 한쪽(깊이)으로 생성해나가기 때문에. */
	/* 이후 애님에ㅣ션 재생할때 뼈의 CombinedTransformation를 만든다. */
	/* CombinedTransformation만들려면 부모의 상태가 모두 갱신되어있어야돼. 왜 부모의 컴바인드 이용하니까.ㄴ ==
	 부모부터 자식으로 순차적으로 CombinedTransformation를 만들어야한다라는 걸 의미.  */
	 /* m_HierarchyNodes컨테이너는 최상위 부모가 가장 앞에 있어야한다. 이놈의 1차 자식들이 두번째에 쫙. 삼차짜식들이 그다음쫘악. */
	 /* 각 노드마다 깊이값(몇차자식이냐? ) 을 저장해두고 나중에 정렬한다. */
	CHierarchyNode* pHierarchyNode = CHierarchyNode::Create(pNode, pParent, iDepth++);

	if (nullptr == pHierarchyNode)
		return E_FAIL;


	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		Ready_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (_uint i = 0; i < m_pAIScene->mNumAnimations; ++i)
	{
		aiAnimation* pAIAnimation = m_pAIScene->mAnimations[i];

		/*I 애니메이션 마다 객체화 하는 이유 : 현재 재생 시간에 맞는 채널들의 뼈 상태를 셋팅한다. (조난 빡세다)
		함수로 만들어야지뭐. */
		CAnimation* pAnimation = CAnimation::Create(pAIAnimation);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}

HRESULT CModel::Ready_Animation_Texture()
{

	if (TYPE::TYPE_NONANIM == m_eModelType)
		return S_OK;

	m_Matrices.resize(m_HierarchyNodes.size());

	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 4096;
	TextureDesc.Height = 2;

	
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pMatrixTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pMatrixTexture, nullptr, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}




CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const wstring& strModelFilePath, const wstring& strModelFileName, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, strModelFilePath, strModelFileName, PivotMatrix)))
	{
		MSG_BOX("Failed To Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CModel* CModel::Create_Bin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const wstring& strModelFilePath, const wstring& strModelFileName, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);
	pInstance->m_bFromBinary = true;
	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);
	if (!m_bFromBinary)
	{
		if (FAILED(pInstance->Initialize(pArg)))
		{
			MSG_BOX("Failed To Cloned : CModel");
			Safe_Release(pInstance);
		}
		return pInstance;
	}
	else
	{
		if (FAILED(pInstance->Initialize_Bin(pArg)))
		{
			MSG_BOX("Failed To Cloned Bin: CModel");
			Safe_Release(pInstance);
		}
		return pInstance;
	}
}

void CModel::Free()
{
	__super::Free();

	for (auto& pHierarchyNode : m_HierarchyNodes)
		Safe_Release(pHierarchyNode);

	m_HierarchyNodes.clear();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(Material.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pMeshContainer : m_Meshes)
		Safe_Release(pMeshContainer);

	m_Meshes.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	m_Importer.FreeScene();
}
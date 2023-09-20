#include "..\Public\Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Shader.h"
#include "AsFileUtils.h"
#include "AsUtils.h"
#include <filesystem>
#include "tinyxml2.h"
#include "ModelConverter.h"

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
	, m_strFilePath(rhs.m_strFilePath)
	, m_strFileName(rhs.m_strFileName)
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

}

CHierarchyNode* CModel::Get_HierarchyNode(const char* pNodeName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)
		{
			return !strcmp(pNodeName, pNode->Get_Name());
		});

	if (iter == m_HierarchyNodes.end())
		return nullptr;

	return *iter;
}

_uint CModel::Get_MaterialIndex(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Get_MaterialIndex();
}

_uint CModel::Get_MaxAnimIndex()
{
	return m_Animations.size() - 1;
}


HRESULT CModel::Initialize_Prototype(TYPE eType, const wstring& strModelFilePath, const wstring& strModelFileName, _fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);
	m_strFilePath = strModelFilePath;
	m_strFileName = strModelFileName;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	if (FAILED(Load_AssetFile_FromBinary()))
	{
		if (FAILED(Load_AssetFile_FromFBX()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::SetUp_OnShader(CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pConstantName)
{
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pTexture[eTextureType]->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CModel::SetUpAnimation_OnShader(CShader* pShader)
{
	KEY_DESC KeyDesc = m_Animations[m_iCurrentAnimIndex]->Get_KeyDesc();

	if (FAILED(pShader->Bind_RawValue("g_CurrKeyFrame", &KeyDesc, sizeof(KEY_DESC))))
		return E_FAIL;

	if (FAILED(m_Animations[m_iCurrentAnimIndex]->SetUpAnimation_OnShader(pShader, "g_CurrAnimMap", "g_CurrKeyFrame")))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Play_Animation(_float fTimeDelta)
{
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return E_FAIL;

	m_Animations[m_iCurrentAnimIndex]->Play_Animation(fTimeDelta);


	return S_OK;
}

HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex, _uint iPassIndex)
{
	pShader->Begin(0);

	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Load_AssetFile_FromFBX()
{
	_tchar szFileName[MAX_PATH];

	_wsplitpath_s(m_strFileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	if(nullptr == m_pConverter)
		m_pConverter = new CModelConverter(m_pDevice, m_pContext, szFileName);

	m_pConverter->Read_AssetFile(m_strFilePath + m_strFileName);

	m_pConverter->Import_ModelData();
	m_pConverter->Import_MaterialData();
	m_pConverter->Import_AnimationData();

	if (FAILED(Load_ModelData_FromConverter(XMLoadFloat4x4(&m_PivotMatrix))))
		return E_FAIL;
	if(FAILED(Load_MaterialData_FromConverter()))
		return E_FAIL;
	if(FAILED(Load_AnimationData_FromConverter(XMLoadFloat4x4(&m_PivotMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Load_AssetFile_FromBinary()
{

	if (FAILED(Load_MaterialData_FromFile()))
		return E_FAIL;

	if (FAILED(Load_ModelData_FromFile(XMLoadFloat4x4(&m_PivotMatrix))))
		return E_FAIL;

	if (m_eModelType == TYPE::TYPE_ANIM)
	{
		if (FAILED(Load_AnimationData_FromFile(XMLoadFloat4x4(&m_PivotMatrix))))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CModel::Export_AssetData()
{
	_tchar szFileName[MAX_PATH];
	_wsplitpath_s(m_strFileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	
	if (nullptr == m_pConverter)
		m_pConverter = new CModelConverter(m_pDevice, m_pContext, szFileName);

	wstring szFilePath = wstring(szFileName) + L"/" + szFileName;

	m_pConverter->Export_ModelData(szFilePath);
	m_pConverter->Export_MaterialData(szFilePath);
	m_pConverter->Export_AnimationData(szFilePath);
	return S_OK;
}

HRESULT CModel::Delete_ModelAnimation(_uint iIndex)
{
	auto iter = m_Animations.begin() + iIndex;

	if (*iter)
	{
		m_pConverter->DeleteAnimation((*iter)->Get_AnimationName());

		Safe_Release(*iter);
		m_Animations.erase(iter);
	}

	m_iCurrentAnimIndex = 0;
	return S_OK;
}

HRESULT CModel::Load_ModelData_FromFile(_fmatrix PivotMatrix)
{
	_tchar szFileName[MAX_PATH];
	_wsplitpath_s(m_strFileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	wstring strfullPath = m_strFilePath + szFileName + L".mesh";
	

	shared_ptr<CAsFileUtils> pFileUtils = make_shared<CAsFileUtils>();
	pFileUtils->Open(strfullPath, FileMode::Read);

	int32 iAnimationCount = pFileUtils->Read<int32>();

	if (iAnimationCount > 0)
		m_eModelType = TYPE::TYPE_ANIM;
	else
		m_eModelType = TYPE::TYPE_NONANIM;

	uint32 iBoneCount = pFileUtils->Read<uint32>();

	for (uint32 i = 0; i < iBoneCount; i++)
	{
		shared_ptr<ModelBone> tBone = make_shared<ModelBone>();
		tBone->iIndex = pFileUtils->Read<int32>();
		tBone->strName = CAsUtils::ToWString(pFileUtils->Read<string>());
		tBone->iParentID = pFileUtils->Read<int32>();
		tBone->matTransform = pFileUtils->Read<Matrix>();

		m_ModelBones.push_back(tBone);
	}

	m_iNumMeshes = pFileUtils->Read<uint32>();

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMeshContainer = CMesh::Create(m_pDevice, m_pContext, m_eModelType, nullptr, this, PivotMatrix);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		pMeshContainer->LoadData_FromMeshFile(m_eModelType, pFileUtils.get(), PivotMatrix);

		m_Meshes.push_back(pMeshContainer);
	}


	// Bone 계층 정보 채우기
	if (m_RootBone == nullptr && m_ModelBones.size() > 0)
	{
		m_RootBone = m_ModelBones[0];

		for (const auto& Bone : m_ModelBones)
		{
			if (Bone->iParentID >= 0)
			{
				Bone->tParent = m_ModelBones[Bone->iParentID];
				Bone->tParent->tChildrens.push_back(Bone);
			}
			else
			{
				Bone->tParent = nullptr;
			}
		}
	}


	return S_OK;
}

HRESULT CModel::Load_MaterialData_FromFile()
{

	_tchar szFileName[MAX_PATH];
	_wsplitpath_s(m_strFileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
	
	wstring strFullPath = m_strFilePath + szFileName + L".xml";
	auto parentPath = filesystem::path(strFullPath).parent_path();

	tinyxml2::XMLDocument* Document = new tinyxml2::XMLDocument();
	tinyxml2::XMLError error = Document->LoadFile(CAsUtils::ToString(strFullPath).c_str());

	if (error != tinyxml2::XML_SUCCESS)
		return E_FAIL;

	tinyxml2::XMLElement* Root = Document->FirstChildElement();
	tinyxml2::XMLElement* MaterialNode = Root->FirstChildElement();

	while (MaterialNode)
	{
		MATERIALDESC		MaterialDesc;
		ZeroMemory(&MaterialDesc, sizeof(MATERIALDESC));

		tinyxml2::XMLElement* Node = nullptr;

		Node = MaterialNode->FirstChildElement();

		strncpy_s(MaterialDesc.strName, Node->GetText(), strlen(Node->GetText()));

		// Diffuse Texture
		Node = Node->NextSiblingElement();
		if (Node->GetText())
		{
			wstring strTexture = CAsUtils::ToWString(Node->GetText());
			if (strTexture.length() > 0)
			{
				wstring szFullPath = m_strFilePath + strTexture;

				MaterialDesc.pTexture[aiTextureType_DIFFUSE] = CTexture::Create(m_pDevice, m_pContext, szFullPath);
				if (nullptr == MaterialDesc.pTexture[aiTextureType_DIFFUSE])
					return E_FAIL;
			}
		}

		// Specular Texture
		Node = Node->NextSiblingElement();
		if (Node->GetText())
		{
			wstring strTexture = CAsUtils::ToWString(Node->GetText());
			if (strTexture.length() > 0)
			{
				wstring szFullPath = m_strFilePath + strTexture;

				MaterialDesc.pTexture[aiTextureType_SPECULAR] = CTexture::Create(m_pDevice, m_pContext, szFullPath);
				if (nullptr == MaterialDesc.pTexture[aiTextureType_SPECULAR])
					return E_FAIL;
			}
		}

		// Normal Texture
		Node = Node->NextSiblingElement();
		if (Node->GetText())
		{
			wstring strTexture = CAsUtils::ToWString(Node->GetText());
			if (strTexture.length() > 0)
			{
				wstring szFullPath = m_strFilePath + strTexture;

				MaterialDesc.pTexture[aiTextureType_NORMALS] = CTexture::Create(m_pDevice, m_pContext, szFullPath);
				if (nullptr == MaterialDesc.pTexture[aiTextureType_NORMALS])
					return E_FAIL;
			}
		}

		// Ambient
		{
			Node = Node->NextSiblingElement();

			MaterialDesc.vAmbient.x = Node->FloatAttribute("R");
			MaterialDesc.vAmbient.y = Node->FloatAttribute("G");
			MaterialDesc.vAmbient.z = Node->FloatAttribute("B");
			MaterialDesc.vAmbient.w = Node->FloatAttribute("A");
		}

		// Diffuse
		{
			Node = Node->NextSiblingElement();

			MaterialDesc.vDiffuse.x = Node->FloatAttribute("R");
			MaterialDesc.vDiffuse.y = Node->FloatAttribute("G");
			MaterialDesc.vDiffuse.z = Node->FloatAttribute("B");
			MaterialDesc.vDiffuse.w = Node->FloatAttribute("A");
		}

		// Specular
		{
			Node = Node->NextSiblingElement();

			MaterialDesc.vSpecular.x = Node->FloatAttribute("R");
			MaterialDesc.vSpecular.y = Node->FloatAttribute("G");
			MaterialDesc.vSpecular.z = Node->FloatAttribute("B");
			MaterialDesc.vSpecular.w = Node->FloatAttribute("A");
		}

		// Emissive
		{
			Node = Node->NextSiblingElement();

			MaterialDesc.vEmissive.x = Node->FloatAttribute("R");
			MaterialDesc.vEmissive.y = Node->FloatAttribute("G");
			MaterialDesc.vEmissive.z = Node->FloatAttribute("B");
			MaterialDesc.vEmissive.w = Node->FloatAttribute("A");
		}

		m_Materials.push_back(MaterialDesc);

		// Next Material
		MaterialNode = MaterialNode->NextSiblingElement();
	}

	m_iNumMaterials = m_Materials.size();
	return S_OK;
}

HRESULT CModel::Load_AnimationData_FromFile(_fmatrix PivotMatrix)
{

	_tchar szFileName[MAX_PATH];
	_wsplitpath_s(m_strFileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	wstring szFullPath = m_strFilePath + szFileName + L".anim";

	shared_ptr<CAsFileUtils> pFileUtils = make_shared<CAsFileUtils>();
	pFileUtils->Open(szFullPath, FileMode::Read);

	m_iNumAnimations = pFileUtils->Read<uint32>();

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(nullptr);
		if (nullptr == pAnimation)
			return E_FAIL;

		if (FAILED(pAnimation->LoadData_FromAnimationFile(pFileUtils.get(), PivotMatrix)))
			return E_FAIL;


		m_Animations.push_back(pAnimation);
	}

	/// Ainmation Texture

	if (FAILED(Create_AnimationTexture(PivotMatrix)))
		return E_FAIL;



	return S_OK;
}

HRESULT CModel::Create_AnimationTexture(_fmatrix PivotMatrix)
{
	if (m_Animations.size() == 0)
		return E_FAIL;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		if (FAILED(m_Animations[i]->Create_Transform(m_pDevice, m_ModelBones, PivotMatrix)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Load_ModelData_FromConverter(_fmatrix PivotMatrix)
{
	vector<shared_ptr<asBone>>* pBones = m_pConverter->Get_Bones();

	if (m_pConverter->Get_Animations()->size() > 0)
		m_eModelType = TYPE::TYPE_ANIM;
	else
		m_eModelType = TYPE::TYPE_NONANIM;

	for (uint32 i = 0; i < pBones->size(); i++)
	{
		shared_ptr<ModelBone> tBone = make_shared<ModelBone>();
		tBone->iIndex = (*pBones)[i]->index;
		tBone->strName = CAsUtils::ToWString((*pBones)[i]->name);
		tBone->iParentID = (*pBones)[i]->parent;
		tBone->matTransform = (*pBones)[i]->transform;

		m_ModelBones.push_back(tBone);
	}

	vector<shared_ptr<asMesh>>* pMeshes = m_pConverter->Get_Meshes();

	m_iNumMeshes = pMeshes->size();

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMeshContainer = CMesh::Create(m_pDevice, m_pContext, m_eModelType, nullptr, this, PivotMatrix);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		pMeshContainer->LoadData_FromConverter(m_eModelType, (*pMeshes)[i], PivotMatrix);

		m_Meshes.push_back(pMeshContainer);
	}


	// Bone 계층 정보 채우기
	if (m_RootBone == nullptr && m_ModelBones.size() > 0)
	{
		m_RootBone = m_ModelBones[0];

		for (const auto& Bone : m_ModelBones)
		{
			if (Bone->iParentID >= 0)
			{
				Bone->tParent = m_ModelBones[Bone->iParentID];
				Bone->tParent->tChildrens.push_back(Bone);
			}
			else
			{
				Bone->tParent = nullptr;
			}
		}
	}


	return S_OK;
}

HRESULT CModel::Load_MaterialData_FromConverter()
{
	vector<shared_ptr<asMaterial>>* pMaterials = m_pConverter->Get_Materials();

	for (_uint i = 0; i < pMaterials->size(); ++i)
	{
		MATERIALDESC		MaterialDesc;
		ZeroMemory(&MaterialDesc, sizeof(MATERIALDESC));

		strcpy_s(MaterialDesc.strName, (*pMaterials)[i]->name.c_str());
		// strncpy_s(MaterialDesc.strName, sizeof(MAX_PATH) ,(*pMaterials)[i]->name.c_str(), (*pMaterials)[i]->name.length() + 1);



		// Diffuse Texture  복사좀 해줘
		wstring strDiffuseTexture = CAsUtils::ToWString((*pMaterials)[i]->diffuseFile);
		if (strDiffuseTexture.length() > 0)
		{
			wstring szFullPath = m_strFilePath + strDiffuseTexture;

			MaterialDesc.pTexture[aiTextureType_DIFFUSE] = CTexture::Create(m_pDevice, m_pContext, szFullPath);
			if (nullptr == MaterialDesc.pTexture[aiTextureType_DIFFUSE])
				return E_FAIL;
		}

		// Specular Texture
		wstring strSpecularTexture = CAsUtils::ToWString((*pMaterials)[i]->specularFile);
		if (strSpecularTexture.length() > 0)
		{
			wstring szFullPath = m_strFilePath + strSpecularTexture;

			MaterialDesc.pTexture[aiTextureType_SPECULAR] = CTexture::Create(m_pDevice, m_pContext, szFullPath);
			if (nullptr == MaterialDesc.pTexture[aiTextureType_SPECULAR])
				return E_FAIL;
		}

		// Normal Texture
		wstring strNormalTexture = CAsUtils::ToWString((*pMaterials)[i]->normalFile);
		if (strNormalTexture.length() > 0)
		{
			wstring szFullPath = m_strFilePath + strNormalTexture;

			MaterialDesc.pTexture[aiTextureType_NORMALS] = CTexture::Create(m_pDevice, m_pContext, szFullPath);
			if (nullptr == MaterialDesc.pTexture[aiTextureType_NORMALS])
				return E_FAIL;
		}

		// Ambient
		MaterialDesc.vAmbient = (*pMaterials)[i]->ambient;

		// Diffuse
		MaterialDesc.vDiffuse = (*pMaterials)[i]->diffuse;

		// Specular
		MaterialDesc.vSpecular = (*pMaterials)[i]->specular;

		// Emissive
		MaterialDesc.vEmissive = (*pMaterials)[i]->emissive;

		m_Materials.push_back(MaterialDesc);

	}

	m_iNumMaterials = m_Materials.size();

	return S_OK;
}

HRESULT CModel::Load_AnimationData_FromConverter(_fmatrix PivotMatrix)
{
	map<const string, shared_ptr<asAnimation>>* pAnimations = m_pConverter->Get_Animations();

	m_iNumAnimations = pAnimations->size();

	for (auto& iter : *pAnimations)
	{
		CAnimation* pAnimation = CAnimation::Create(nullptr);
		if (nullptr == pAnimation)
			return E_FAIL;

		if (FAILED(pAnimation->LoadData_FromConverter(iter.second, PivotMatrix)))
			return E_FAIL;


		m_Animations.push_back(pAnimation);
	}

	/// Ainmation Texture

	if (FAILED(Create_AnimationTexture(PivotMatrix)))
		return E_FAIL;



	return S_OK;
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

HRESULT CModel::Ready_Materials(const char* pModelFilePath)
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

			char			szFullPath[MAX_PATH] = "";
			char			szFileName[MAX_PATH] = "";
			char			szExt[MAX_PATH] = "";

			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar			szWideFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szWideFullPath, MAX_PATH);


			MaterialDesc.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szWideFullPath);
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


CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const wstring& strFolderPath, const wstring& strFileName, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, strFolderPath, strFileName, PivotMatrix)))
	{
		MSG_BOX("Failed To Created : CTexture");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
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

	Safe_Delete(m_pConverter);
}

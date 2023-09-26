#include "..\Public\Data_Manager.h"
#include "Model.h"
#include <filesystem>
#include "tinyxml2.h"
#include "FileUtils.h"
#include "HierarchyNode.h"
#include "Utils.h"
#include "Mesh.h"
#include "Texture.h"
#include "Animation.h"
#include "Channel.h"

IMPLEMENT_SINGLETON(CData_Manager);


CData_Manager::CData_Manager()
{

}

HRESULT CData_Manager::Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);

	return S_OK;
}

HRESULT CData_Manager::Export_Model_Data(CModel* pModel, const wstring& strSubFolderName, const wstring& strFileName)
{
	_tchar szOriginFileName[MAX_PATH];
	_wsplitpath_s(strFileName.c_str(), nullptr, 0, nullptr, 0, szOriginFileName, MAX_PATH, nullptr, 0);


	wstring strFinalFolderPath = m_strExportFolderPath + strSubFolderName + szOriginFileName + L"/" + szOriginFileName;


	if (FAILED(Export_Mesh(strFinalFolderPath, pModel)))
		return E_FAIL;

	if (FAILED(Export_Material(strFinalFolderPath, pModel)))
		return E_FAIL;

	if (CModel::TYPE::TYPE_NONANIM == pModel->Get_ModelType())
		return S_OK;

	if (FAILED(Export_Animation(strFinalFolderPath, pModel)))
		return E_FAIL;

	return S_OK;
}

#pragma region Import_ModelData
CModel* CData_Manager::Import_Model_Data(_uint eType, wstring strFolderPath, wstring strFileName, _fmatrix PivotMatrix)
{
	_tchar szFileName[MAX_PATH];
	_tchar szExt[MAX_PATH];
	_wsplitpath_s(strFileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);


	CModel* pModel = nullptr;
	if (0 == lstrcmp(szExt, L".fbx"))
	{
		pModel = CModel::Create(
			m_pDevice, 
			m_pContext, CModel::TYPE(eType), 
			strFolderPath, strFileName, PivotMatrix);

		if (nullptr == pModel)
			return nullptr;

		if (FAILED(pModel->Initialize(nullptr)))
		{
			MSG_BOX("Model Initialize Failed : Data Manager");
			Safe_Release(pModel);
			return nullptr;
		}

		return pModel;
	}
	else
	{
		pModel = CModel::Create_Bin(
			m_pDevice,
			m_pContext, CModel::TYPE(eType),
			strFolderPath, strFileName, PivotMatrix);

		if (nullptr == pModel)
			return nullptr;

		wstring strFinalFolderPath = strFolderPath + szFileName;

		if (FAILED(Import_Mesh(strFinalFolderPath, pModel)))
		{
			MSG_BOX("Import_Mesh Failed.");
			Safe_Release(pModel);
			return nullptr;
		}
			

		if (FAILED(Import_Material(strFinalFolderPath, strFolderPath, pModel)))
		{
			MSG_BOX("Import_Material Failed.");
			Safe_Release(pModel);
			return nullptr;
		}

		if (CModel::TYPE::TYPE_NONANIM == pModel->Get_ModelType())
			return pModel;

		if (FAILED(Import_Animation(strFinalFolderPath, pModel)))
		{
			MSG_BOX("Import_Animation Failed.");
			Safe_Release(pModel);
			return nullptr;
		}
	}

	return pModel;
		
	
}
#pragma endregion

#pragma region Export_Mesh
HRESULT CData_Manager::Export_Mesh(const wstring& strFinalFolderPath, CModel* pModel)
{
	if (pModel == nullptr)
		return E_FAIL;

	wstring strMeshFilePath = strFinalFolderPath + L".mesh";
	auto path = filesystem::path(strMeshFilePath);

	filesystem::create_directories(path.parent_path());

	CUtils* StrUtil = CUtils::GetInstance();
	Safe_AddRef(StrUtil);

	shared_ptr<CFileUtils> File = make_shared<CFileUtils>();
	File->Open(strMeshFilePath, FileMode::Write);

	// Start
	File->Write<_uint>(_uint(pModel->m_Animations.size()));

	File->Write<_uint>(_uint(pModel->m_HierarchyNodes.size()));
	for (auto& Node : pModel->m_HierarchyNodes)
	{
		File->Write<_uint>(Node->m_iDepth);
		File->Write<string>(StrUtil->wstring_to_string(Node->m_strName));
		File->Write<string>(StrUtil->wstring_to_string(Node->Get_ParentName()));
		// Warning ! if Add Parent
		File->Write<_float4x4>(Node->m_Transformation);
		File->Write<_float4x4>(Node->m_OffsetMatrix);
	}

	File->Write<_uint>(pModel->m_iNumMeshes);
	
	for (auto& Mesh : pModel->m_Meshes)
	{
		File->Write<string>(StrUtil->wstring_to_string(Mesh->m_strName));
		File->Write<_uint>(Mesh->m_iMaterialIndex);
		File->Write<_uint>(Mesh->m_iNumBones);
		File->Write<_uint>(Mesh->m_iNumVertexBuffers);
		File->Write<_uint>(Mesh->m_iNumVertices);
		File->Write<_uint>(Mesh->m_iStride);
		File->Write<_uint>(Mesh->m_iNumPrimitives);

		File->Write<_uint>(Mesh->m_iIndexSizeofPrimitive);
		File->Write<_uint>(Mesh->m_iNumIndicesofPrimitive);
		File->Write<D3D11_PRIMITIVE_TOPOLOGY>(Mesh->m_eTopology);
		File->Write<DXGI_FORMAT>(Mesh->m_eIndexFormat);


		File->Write(&Mesh->m_pIB[0], sizeof(FACEINDICES32) * Mesh->m_iNumPrimitives);


		if (pModel->m_eModelType == CModel::TYPE::TYPE_ANIM)
			File->Write(&Mesh->m_pVB[0], sizeof(VTXANIMMODEL) * Mesh->m_iNumVertices);
		else
			File->Write(&Mesh->m_pVB[0], sizeof(VTXMODEL) * Mesh->m_iNumVertices);




	}

	Safe_Release(StrUtil);
	return S_OK;
}
#pragma endregion

#pragma region Export_Material

HRESULT CData_Manager::Export_Material(const wstring& strFinalFolderPath, CModel* pModel)
{
	if (pModel == nullptr)
		return E_FAIL;

	wstring strMaterialFilePath = strFinalFolderPath + L".xml";
	auto path = filesystem::path(strMaterialFilePath);
	filesystem::create_directories(path.parent_path());

	CUtils* StringUtil = CUtils::GetInstance();
	Safe_AddRef(StringUtil);
	
	shared_ptr<tinyxml2::XMLDocument> Document = make_shared<tinyxml2::XMLDocument>();
	tinyxml2::XMLDeclaration* Decl = Document->NewDeclaration();
	Document->LinkEndChild(Decl);

	tinyxml2::XMLElement* Root = Document->NewElement("Materials");
	Document->LinkEndChild(Root);

	for (auto& Material : pModel->m_Materials)
	{
		tinyxml2::XMLElement* Node = Document->NewElement("Material");
		Root->LinkEndChild(Node);

		tinyxml2::XMLElement* Element = nullptr;

		Element = Document->NewElement("Name");
		Element->SetText(Material.strName);
		Node->LinkEndChild(Element);

		Element = Document->NewElement("Diffuse_Texture");
		Element->SetText(Export_Texture(pModel->m_strFolderPath, 
										path.parent_path().string(), 
										Material.pTexture[aiTextureType_DIFFUSE]).c_str());
		Node->LinkEndChild(Element);

		Element = Document->NewElement("Specular_Texture");
		Element->SetText(Export_Texture(pModel->m_strFolderPath, 
										path.parent_path().string(), 
										Material.pTexture[aiTextureType_SPECULAR]).c_str());
		Node->LinkEndChild(Element);

		Element = Document->NewElement("Normal_Texture");
		Element->SetText(Export_Texture(pModel->m_strFolderPath, 
										path.parent_path().string(), 
										Material.pTexture[aiTextureType_NORMALS]).c_str());
		Node->LinkEndChild(Element);

		Element = Document->NewElement("Emissive_Texture");
		Element->SetText(Export_Texture(pModel->m_strFolderPath, 
										path.parent_path().string(), 
										Material.pTexture[aiTextureType_EMISSIVE]).c_str());
		Node->LinkEndChild(Element);

	}

	Document->SaveFile(StringUtil->wstring_to_string(strMaterialFilePath).c_str());
	Safe_Release(StringUtil);
	

	return S_OK;
}
#pragma endregion

#pragma region Export_Animation
HRESULT CData_Manager::Export_Animation(const wstring& strFinalFolderPath, CModel* pModel)
{
	if (pModel == nullptr)
		return E_FAIL;

	wstring strAnimationFilePath = strFinalFolderPath + L".anim";
	auto path = filesystem::path(strAnimationFilePath);
	filesystem::create_directories(path.parent_path());

	CUtils* StringUtil = CUtils::GetInstance();
	Safe_AddRef(StringUtil);

	shared_ptr<CFileUtils> File = make_shared<CFileUtils>();
	File->Open(strAnimationFilePath, FileMode::Write);

	File->Write<_uint>(pModel->m_Animations.size());
	for (auto& Animation : pModel->m_Animations)
	{
		File->Write<string>(StringUtil->wstring_to_string(Animation->m_strName));
		File->Write<_float>(Animation->m_fDuration);
		File->Write<_float>(Animation->m_fTickPerSecond);
		File->Write<_float>(Animation->m_fSpeed);
		
		File->Write<_uint>(Animation->m_iNumChannels);
		for (auto& Channel : Animation->m_Channels)
		{
			File->Write<string>(StringUtil->wstring_to_string(Channel->m_strName));
			for (_uint i = 0; i < Channel->m_iNumKeyFrames; ++i)			
				File->Write(&Channel->m_KeyFrames[i], sizeof(KEYFRAME));
		}

	}

	Safe_Release(StringUtil);
	return S_OK;
}

#pragma endregion


#pragma region Export_Texture
string CData_Manager::Export_Texture(const wstring& strOriginFolder, const string& strSaveFolder, CTexture* pTexture, _uint iIdx)
{
	if (pTexture == nullptr)
		return "";


	string TextureName = CUtils::GetInstance()->wstring_to_string(pTexture->Get_Name(iIdx));
	
	string OriginTextureFilePath = CUtils::GetInstance()->wstring_to_string(strOriginFolder) + TextureName;
	string SaveTextureFilePath = strSaveFolder + "/" + TextureName;

	CopyFileA(OriginTextureFilePath.c_str(), SaveTextureFilePath.c_str(), false);

	return TextureName;
}

#pragma endregion

#pragma region Import_Mesh
HRESULT CData_Manager::Import_Mesh(const wstring strFinalPath, CModel* pModel)
{
	wstring strMeshFilePath = strFinalPath + L".mesh";
	shared_ptr<CFileUtils> File = make_shared<CFileUtils>();
	File->Open(strMeshFilePath, FileMode::Read);


	CUtils* StrUtil = CUtils::GetInstance();

	_uint iAnimationCount = File->Read<_uint>();

	pModel->m_eModelType = iAnimationCount > 0 ? CModel::TYPE::TYPE_ANIM : CModel::TYPE::TYPE_NONANIM;
	pModel->m_iNumAnimations = iAnimationCount;

	_uint iNumHierarchyNodes = File->Read<_uint>(); 
	for (_uint i = 0; i < iNumHierarchyNodes; ++i)
	{
		CHierarchyNode* pNode = CHierarchyNode::Create_Bin();
		
		pNode->m_iDepth = File->Read<_uint>();
		pNode->m_strName = StrUtil->string_to_wstring(File->Read<string>());
		pNode->m_strParentName = StrUtil->string_to_wstring(File->Read<string>());
		// Warning ! if Add Parent
		pNode->m_Transformation = File->Read<_float4x4>();
		pNode->m_OffsetMatrix = File->Read<_float4x4>();


		pModel->m_HierarchyNodes.push_back(pNode);
	}


	// 부모를 찾아 링크.
	for (auto& Node : pModel->m_HierarchyNodes)
		Node->Initialize_Bin(pModel);


	pModel->m_iNumMeshes = File->Read<_uint>();
	for (_uint i = 0; i < pModel->m_iNumMeshes; ++i)
	{
		
		CMesh* Mesh = CMesh::Create_Bin(m_pDevice, m_pContext, pModel->m_eModelType, XMLoadFloat4x4(&pModel->m_PivotMatrix));

		Mesh->m_strName = StrUtil->string_to_wstring(File->Read<string>());
		File->Read<_uint>(Mesh->m_iMaterialIndex);
		File->Read<_uint>(Mesh->m_iNumBones);
		File->Read<_uint>(Mesh->m_iNumVertexBuffers);
		File->Read<_uint>(Mesh->m_iNumVertices);
		File->Read<_uint>(Mesh->m_iStride);
		File->Read<_uint>(Mesh->m_iNumPrimitives);

		File->Read<_uint>(Mesh->m_iIndexSizeofPrimitive);
		File->Read<_uint>(Mesh->m_iNumIndicesofPrimitive);
		File->Read<D3D11_PRIMITIVE_TOPOLOGY>(Mesh->m_eTopology);
		File->Read<DXGI_FORMAT>(Mesh->m_eIndexFormat);



		vector<FACEINDICES32> Indices(Mesh->m_iNumPrimitives);
		for (_uint j = 0; j < Mesh->m_iNumPrimitives; ++j)
		{
			FACEINDICES32 Index;
			File->Read<FACEINDICES32>(Index);
			Indices.push_back(Index);
		}


		if (pModel->m_eModelType == CModel::TYPE::TYPE_ANIM)
		{
			vector<VTXANIMMODEL> Vertices(Mesh->m_iNumVertices);
			for (_uint j = 0; j < Mesh->m_iNumVertices; ++j)
			{
				VTXANIMMODEL Vertex;
				ZeroMemory(&Vertex, sizeof(VTXANIMMODEL));
				File->Read<VTXANIMMODEL>(Vertex);

				Vertices.push_back(Vertex);
			}
			if(FAILED(Mesh->Ready_AnimVertices(Vertices, Indices)))
				return E_FAIL;
		}

		else
		{
			vector<VTXMODEL> Vertices(Mesh->m_iNumVertices);
			for (_uint j = 0; j < Mesh->m_iNumVertices; ++j)
			{
				VTXMODEL Vertex;
				ZeroMemory(&Vertex, sizeof(VTXMODEL));
				File->Read<VTXMODEL>(Vertex);

				Vertices.push_back(Vertex);
			}
			if (FAILED(Mesh->Ready_Vertices(Vertices, Indices)))
				return E_FAIL;
		}
	}


	return S_OK;
}
#pragma endregion


#pragma region Import_Material
HRESULT CData_Manager::Import_Material(const wstring strFinalPath, const wstring strFolderPath, CModel* pModel)
{
	if (pModel == nullptr)
		return E_FAIL;

	wstring strMaterialFilePath = strFinalPath + L".xml";
	
	CUtils* StringUtil = CUtils::GetInstance();
	Safe_AddRef(StringUtil);

	tinyxml2::XMLDocument* Document = new tinyxml2::XMLDocument();
	tinyxml2::XMLError error = Document->LoadFile(StringUtil->wstring_to_string(strMaterialFilePath).c_str());

	if (error != tinyxml2::XML_SUCCESS)
		return E_FAIL;

	tinyxml2::XMLElement* Root = Document->FirstChildElement();
	tinyxml2::XMLElement* MaterialNode = Root->FirstChildElement();

	while (MaterialNode)
	{
		MATERIALDESC MaterialDesc;
		ZeroMemory(&MaterialDesc, sizeof(MATERIALDESC));

		tinyxml2::XMLElement* Node = nullptr;
		Node = MaterialNode->FirstChildElement();

		lstrcpy(MaterialDesc.strName, StringUtil->string_to_wstring(Node->GetText()).c_str());


		// Diffuse Texture
		Node = Node->NextSiblingElement();
		if (Node->GetText())
		{
			wstring strTexture = StringUtil->string_to_wstring(Node->GetText());
			if (strTexture.length() > 0)
			{
				MaterialDesc.pTexture[aiTextureType_DIFFUSE] = CTexture::Create(m_pDevice, m_pContext, strFolderPath + strTexture);
				if (nullptr == MaterialDesc.pTexture[aiTextureType_DIFFUSE])
					return E_FAIL;
			}
		}

		// Specular
		Node = Node->NextSiblingElement();
		if (Node->GetText())
		{
			wstring strTexture = StringUtil->string_to_wstring(Node->GetText());
			if (strTexture.length() > 0)
			{
				MaterialDesc.pTexture[aiTextureType_SPECULAR] = CTexture::Create(m_pDevice, m_pContext, strFolderPath + strTexture);
				if (nullptr == MaterialDesc.pTexture[aiTextureType_SPECULAR])
					return E_FAIL;
			}
		}


		// Normal
		Node = Node->NextSiblingElement();
		if (Node->GetText())
		{
			wstring strTexture = StringUtil->string_to_wstring(Node->GetText());
			if (strTexture.length() > 0)
			{
				MaterialDesc.pTexture[aiTextureType_NORMALS] = CTexture::Create(m_pDevice, m_pContext, strFolderPath + strTexture);
				if (nullptr == MaterialDesc.pTexture[aiTextureType_NORMALS])
					return E_FAIL;
			}
		}

		// Emissive
		Node = Node->NextSiblingElement();
		if (Node->GetText())
		{
			wstring strTexture = StringUtil->string_to_wstring(Node->GetText());
			if (strTexture.length() > 0)
			{
				MaterialDesc.pTexture[aiTextureType_EMISSIVE] = CTexture::Create(m_pDevice, m_pContext, strFolderPath + strTexture);
				if (nullptr == MaterialDesc.pTexture[aiTextureType_EMISSIVE])
					return E_FAIL;
			}
		}
		

		pModel->m_Materials.push_back(MaterialDesc);
		MaterialNode = MaterialNode->NextSiblingElement();
	}
	pModel->m_iNumMaterials = pModel->m_Materials.size();


	Safe_Release(StringUtil);
	return S_OK;
}
#pragma endregion

HRESULT CData_Manager::Import_Animation(const wstring strFinalPath, CModel* pModel)
{
	//230926 TODO : Import_Animation
	return S_OK;
}

HRESULT CData_Manager::Import_Texture(const wstring strFinalPath, CModel* pModel)
{
	return S_OK;
}



void CData_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

#include "ModelConverter.h"
#include "Engine_Defines.h"
#include <filesystem>
#include "AsUtils.h"
#include "AsFileUtils.h"
#include "tinyxml2.h"


CModelConverter::CModelConverter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFileName)
{
	_importer = make_shared<Assimp::Importer>();

	m_strFileName = strFileName;
	m_pDevice = pDevice;
	m_pContext = pContext;

}

CModelConverter::~CModelConverter()
{
}

void CModelConverter::Read_AssetFile(wstring file)
{
	auto p = std::filesystem::path(file);
	if (false == std::filesystem::exists(p))
	{
		MSG_BOX("FAILED : File Path Strange.");
		return;
	}
	

	_scene = _importer->ReadFile(
		CAsUtils::ToString(file),
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	assert(_scene != nullptr);
}

void CModelConverter::Export_ModelData(wstring savePath)
{
	wstring finalPath = _modelPath + savePath + L".mesh";

	//Write CSV File
	{
		FILE* file;
		::fopen_s(&file, "../Vertices.csv", "w");

		for (shared_ptr<asBone>& bone : _bones)
		{
			string name = bone->name;
			::fprintf(file, "%d,%s\n", bone->index, bone->name.c_str());
		}

		::fprintf(file, "\n");

		for (shared_ptr<asMesh>& mesh : _meshes)
		{
			string name = mesh->name;
			::printf("%s\n", name.c_str());

			for (UINT i = 0; i < mesh->vertices.size(); i++)
			{
				Vec3 p = mesh->vertices[i].vPosition;
				XMUINT4 indices = mesh->vertices[i].vBlendIndex;
				Vec4 weights = mesh->vertices[i].vBlendWeight;

				::fprintf(file, "%f,%f,%f,", p.x, p.y, p.z);
				::fprintf(file, "%f,%f,%f,%f,", indices.x, indices.y, indices.z, indices.w);
				::fprintf(file, "%f,%f,%f,%f\n", weights.x, weights.y, weights.z, weights.w);
			}
		}

		::fclose(file);
	}


	Write_ModelFile(finalPath);
}

void CModelConverter::Export_MaterialData(wstring savePath)
{
	wstring finalPath = _texturePath + savePath + L".xml";
	Write_MaterialData(finalPath);
}

void CModelConverter::Export_AnimationData(wstring savePath)
{
	_uint iAniCount = _animations.size();

	wstring finalPath = _modelPath + savePath + L".anim";
	if (iAniCount > 0)
	{
		Write_AnimationData(finalPath);
	}
}

void CModelConverter::Import_ModelData()
{
	Read_ModelData(_scene->mRootNode, -1, -1);
	Set_BoneOffsetMatrix();
	Read_SkinData();
}

void CModelConverter::Import_MaterialData()
{
	Read_MaterialData();
}

void CModelConverter::Import_AnimationData()
{
	_uint iAniCount = _scene->mNumAnimations;

	_animations.reserve(_scene->mNumAnimations);

	if (iAniCount > 0)
	{
		for (_uint i = 0; i < iAniCount; ++i)
		{
			shared_ptr<asAnimation> animation = Read_AnimationData(_scene->mAnimations[i]);

			_animations.push_back(animation);
		}
	}
}

void CModelConverter::Read_ModelData(aiNode* node, _uint index, int32 parent)
{
	shared_ptr<asBone> bone = make_shared<asBone>();
	bone->index = index;
	bone->parent = parent;
	bone->name = node->mName.C_Str();

	// Relative Transform
	Matrix transform(node->mTransformation[0]);
	bone->transform = transform.Transpose();

	//// 2) Root (Local)
	//Matrix matParent = Matrix::Identity;
	//if (parent >= 0)
	//	matParent = _bones[parent]->transform;

	//// Local (Root) Transform
	//bone->transform = bone->transform * matParent;

	bone->transform = Matrix::Identity;

	_bones.push_back(bone);

	// Mesh
	Read_MeshData(node, index);

	// 재귀 함수
	for (uint32 i = 0; i < node->mNumChildren; i++)
		Read_ModelData(node->mChildren[i], _bones.size(), index);
}

void CModelConverter::Read_MeshData(aiNode* node, _uint bone)
{
	if (node->mNumMeshes < 1)
		return;

	for (uint32 i = 0; i < node->mNumMeshes; i++)
	{
		shared_ptr<asMesh> mesh = make_shared<asMesh>();
		mesh->name = node->mName.C_Str();
		mesh->boneIndex = bone;

		uint32 index = node->mMeshes[i];
		aiMesh* srcMesh = _scene->mMeshes[index];
		mesh->mesh = srcMesh;

		// Material Name
		aiMaterial* material = _scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->materialName = material->GetName().C_Str();
		mesh->materialIndex = srcMesh->mMaterialIndex;

		for (uint32 v = 0; v < srcMesh->mNumVertices; v++)
		{
			// Vertex
			VertexType vertex;
			::memcpy(&vertex.vPosition, &srcMesh->mVertices[v], sizeof(Vec3));

			// UV
			if (srcMesh->HasTextureCoords(0))
				::memcpy(&vertex.vTexture, &srcMesh->mTextureCoords[0][v], sizeof(Vec2));

			// Normal
			if (srcMesh->HasNormals())
				::memcpy(&vertex.vNormal, &srcMesh->mNormals[v], sizeof(Vec3));

			mesh->vertices.push_back(vertex);
		}

		// Index
		for (uint32 f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (uint32 k = 0; k < 3; k++)
				mesh->indices.push_back(face.mIndices[k]);
		}

		_meshes.push_back(mesh);
	}
}

void CModelConverter::Read_SkinData()
{
	for (uint32 i = 0; i < _scene->mNumMeshes; i++)
	{
		aiMesh* srcMesh = _scene->mMeshes[i];
		if (srcMesh->HasBones() == false)
			continue;

		shared_ptr<asMesh> mesh = _meshes[i];

		vector<asBoneWeights> tempVertexBoneWeights;
		tempVertexBoneWeights.resize(mesh->vertices.size());

		_uint iTemp = 0;
		// Bone을 순회하면서 연관된 VertexId, Weight를 구해서 기록한다.
		for (uint32 b = 0; b < srcMesh->mNumBones; b++)
		{
			aiBone* srcMeshBone = srcMesh->mBones[b];
			uint32 boneIndex = GetBoneIndex(srcMeshBone->mName.C_Str());

			for (uint32 w = 0; w < srcMeshBone->mNumWeights; w++)
			{
				uint32 index = srcMeshBone->mWeights[w].mVertexId;
				float weight = srcMeshBone->mWeights[w].mWeight;
				tempVertexBoneWeights[index].AddWeights(boneIndex, weight);
			}
		}

		// 최종 결과 계산
		for (uint32 v = 0; v < tempVertexBoneWeights.size(); v++)
		{
			tempVertexBoneWeights[v].Normalize();

			asBlendWeight blendWeight = tempVertexBoneWeights[v].GetBlendWeights();
			mesh->vertices[v].vBlendIndex = blendWeight.indices;
			mesh->vertices[v].vBlendWeight = blendWeight.weights;
		}

		iTemp = 0;
	}

	_uint iTemp = 0;
}

void CModelConverter::Write_ModelFile(wstring finalPath)
{
	auto path = filesystem::path(finalPath);

	// 폴더가 없으면 만든다.
	filesystem::create_directories(path.parent_path());

	shared_ptr<CAsFileUtils> file = make_shared<CAsFileUtils>();
	file->Open(finalPath, FileMode::Write);
	file->Write<int32>(_scene->mNumAnimations);

	// Bone Data
	file->Write<uint32>(_bones.size());
	for (shared_ptr<asBone>& bone : _bones)
	{
		file->Write<int32>(bone->index);
		file->Write<string>(bone->name);
		file->Write<int32>(bone->parent);
		file->Write<Matrix>(bone->transform);
	}

	// Mesh Data
	file->Write<uint32>(_meshes.size());
	for (shared_ptr<asMesh>& meshData : _meshes)
	{
		file->Write<string>(meshData->name);
		file->Write<int32>(meshData->boneIndex);
		file->Write<string>(meshData->materialName);
		file->Write<int32>(meshData->materialIndex);

		// Vertex Data
		file->Write<uint32>(meshData->vertices.size());
		file->Write(&meshData->vertices[0], sizeof(VertexType) * meshData->vertices.size());

		// Index Data
		file->Write<uint32>(meshData->indices.size());
		file->Write(&meshData->indices[0], sizeof(uint32) * meshData->indices.size());
	}
}

shared_ptr<asAnimation> CModelConverter::Find_Animation(const wstring& strAnimation)
{
	for (auto& iter : _animations)
	{
		if (iter->name == CAsUtils::ToString(strAnimation))
			return iter;
	}

	return nullptr;
}

HRESULT CModelConverter::DeleteAnimation(const wstring& strAnimation)
{
	for (auto iter = _animations.begin(); iter != _animations.end(); ++iter)
	{
		
		if ((*iter)->name == CAsUtils::ToString(strAnimation))
		{
			_animations.erase(iter);
			return S_OK;
		}
	}
	

	return E_FAIL;
}

HRESULT CModelConverter::Set_AnimationName(const wstring& strNewName, _uint iIndex)
{
	if (iIndex >= _animations.size())
		return E_FAIL;

	_animations[iIndex]->name = CAsUtils::ToString(strNewName);
	return S_OK;
}

HRESULT CModelConverter::Swap_Animation(_uint& iSrc, _uint& iDest)
{
	if (iDest >= _animations.size())
		iDest = _animations.size() - 1;
	if (iDest < 0)
		iDest = 0;

	shared_ptr<asAnimation> Temp = _animations[iSrc];
	_animations[iSrc] = _animations[iDest];
	_animations[iDest] = Temp;

	return S_OK;
}

void CModelConverter::Read_MaterialData()
{
	for (uint32 i = 0; i < _scene->mNumMaterials; i++)
	{
		aiMaterial* srcMaterial = _scene->mMaterials[i];

		shared_ptr<asMaterial> material = make_shared<asMaterial>();
		material->name = srcMaterial->GetName().C_Str();

		aiColor3D color;
		// Ambient
		srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
		material->ambient = _float4(color.r, color.g, color.b, 1.f);

		// Diffuse
		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->diffuse = _float4(color.r, color.g, color.b, 1.f);

		// Specular
		srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material->specular = _float4(color.r, color.g, color.b, 1.f);
		srcMaterial->Get(AI_MATKEY_SHININESS, material->specular.w);

		// Emissive
		srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		material->emissive = _float4(color.r, color.g, color.b, 1.0f);

		aiString file;

		// Diffuse Texture
		{
			srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);

			char szFileName[MAX_PATH] = "";
			char szExt[MAX_PATH] = "";

			_splitpath_s(file.C_Str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			string strTexture = szFileName;
			strTexture += szExt;

			material->diffuseFile = strTexture;
		}
		

		// Specular Texture
		{
			srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);

			char szFileName[MAX_PATH] = "";
			char szExt[MAX_PATH] = "";

			_splitpath_s(file.C_Str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			string strTexture = szFileName;
			strTexture += szExt;

			material->specularFile = strTexture;
		}

		// Normal Texture
		{
			srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);

			char szFileName[MAX_PATH] = "";
			char szExt[MAX_PATH] = "";

			_splitpath_s(file.C_Str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			string strTexture = szFileName;
			strTexture += szExt;

			material->normalFile = strTexture;
		}

		_materials.push_back(material);
	}
}

void CModelConverter::Write_MaterialData(wstring finalPath)
{
	auto path = filesystem::path(finalPath);

	// 폴더가 없으면 만든다.
	filesystem::create_directories(path.parent_path());

	string folder = path.parent_path().string();

	shared_ptr<tinyxml2::XMLDocument> document = make_shared<tinyxml2::XMLDocument>();

	tinyxml2::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	tinyxml2::XMLElement* root = document->NewElement("Materials");
	document->LinkEndChild(root);

	for (shared_ptr<asMaterial> material : _materials)
	{
		tinyxml2::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		tinyxml2::XMLElement* element = nullptr;

		element = document->NewElement("Name");
		element->SetText(material->name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		element->SetText(Write_Texture(folder, material->diffuseFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		element->SetText(Write_Texture(folder, material->specularFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		element->SetText(Write_Texture(folder, material->normalFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->ambient.x);
		element->SetAttribute("G", material->ambient.y);
		element->SetAttribute("B", material->ambient.z);
		element->SetAttribute("A", material->ambient.w);
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->diffuse.x);
		element->SetAttribute("G", material->diffuse.y);
		element->SetAttribute("B", material->diffuse.z);
		element->SetAttribute("A", material->diffuse.w);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		element->SetAttribute("R", material->specular.x);
		element->SetAttribute("G", material->specular.y);
		element->SetAttribute("B", material->specular.z);
		element->SetAttribute("A", material->specular.w);
		node->LinkEndChild(element);

		element = document->NewElement("Emissive");
		element->SetAttribute("R", material->emissive.x);
		element->SetAttribute("G", material->emissive.y);
		element->SetAttribute("B", material->emissive.z);
		element->SetAttribute("A", material->emissive.w);
		node->LinkEndChild(element);
	}

	document->SaveFile(CAsUtils::ToString(finalPath).c_str());
}

string CModelConverter::Write_Texture(string saveFolder, string file)
{
	string fileName = filesystem::path(file).filename().string();
	string folderName = filesystem::path(saveFolder).filename().string();

	const aiTexture* srcTexture = _scene->GetEmbeddedTexture(file.c_str());
	if (srcTexture)
	{
		string pathStr = (filesystem::path(saveFolder) / fileName).string();

		if (srcTexture->mHeight == 0)
		{
			/*shared_ptr<CAsFileUtils> file = make_shared<CAsFileUtils>();
			file->Open(CAsUtils::ToWString(pathStr), FileMode::Write);
			file->Write(srcTexture->pcData, srcTexture->mWidth);*/
		}
		else
		{
			//D3D11_TEXTURE2D_DESC desc;
			//ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
			//desc.Width = srcTexture->mWidth;
			//desc.Height = srcTexture->mHeight;
			//desc.MipLevels = 1;
			//desc.ArraySize = 1;
			//desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			//desc.SampleDesc.Count = 1;
			//desc.SampleDesc.Quality = 0;
			//desc.Usage = D3D11_USAGE_IMMUTABLE;

			//D3D11_SUBRESOURCE_DATA subResource = { 0 };
			//subResource.pSysMem = srcTexture->pcData;

			//ID3D11Texture2D* texture = nullptr;
			//HRESULT hr = DEVICE->CreateTexture2D(&desc, &subResource, texture.GetAddressOf());
			//CHECK(hr);

			//DirectX::ScratchImage img;
			//::CaptureTexture(DEVICE.Get(), DC.Get(), texture.Get(), img);

			//// Save To File
			//hr = DirectX::SaveToDDSFile(*img.GetImages(), DirectX::DDS_FLAGS_NONE, Utils::ToWString(fileName).c_str());
			//CHECK(hr);
		}
	}
	else
	{
		string originStr = (filesystem::path(_assetPath) / folderName / file).string();
		CAsUtils::Replace(originStr, "\\", "/");

		string pathStr = (filesystem::path(saveFolder) / fileName).string();
		CAsUtils::Replace(pathStr, "\\", "/");

		::CopyFileA(originStr.c_str(), pathStr.c_str(), false);
	}

	return fileName;
}

shared_ptr<asAnimation> CModelConverter::Read_AnimationData(aiAnimation* srcAnimation)
{
	shared_ptr<asAnimation> animation = make_shared<asAnimation>();
	animation->name = srcAnimation->mName.C_Str();
	animation->frameRate = (float)srcAnimation->mTicksPerSecond;
	animation->frameCount = (uint32)srcAnimation->mDuration;

	map<string, shared_ptr<asAnimationNode>> cacheAnimNodes;

	for (uint32 i = 0; i < srcAnimation->mNumChannels; i++)
	{
		aiNodeAnim* srcNode = srcAnimation->mChannels[i];

		// 애니메이션 노드 데이터 파싱
		shared_ptr<asAnimationNode> node = Parse_AnimationNode(animation, srcNode);

		// 현재 찾은 노드 중에 제일 긴 시간으로 애니메이션 시간 갱신
		animation->duration = max(animation->duration, node->keyframe.back().time);

		cacheAnimNodes[srcNode->mNodeName.C_Str()] = node;
	}

	Read_KeyframeData(animation, _scene->mRootNode, cacheAnimNodes);

	return animation;
}

shared_ptr<asAnimationNode> CModelConverter::Parse_AnimationNode(shared_ptr<asAnimation> animation, aiNodeAnim* srcNode)
{
	std::shared_ptr<asAnimationNode> node = make_shared<asAnimationNode>();
	node->name = srcNode->mNodeName;

	uint32 keyCount = max(max(srcNode->mNumPositionKeys, srcNode->mNumScalingKeys), srcNode->mNumRotationKeys);

	if (keyCount >= 5 && srcNode->mNumPositionKeys != srcNode->mNumScalingKeys)
		int i = 0;


	asKeyframeData frameData;
	uint32 iPositionIndex = 0;
	uint32 iRotationIndex = 0;
	uint32 iScaleIndex = 0;

	for (uint32 k = 0; k < animation->frameCount; k++)
	{
		// Position
		if (::fabsf((float)srcNode->mPositionKeys[iPositionIndex].mTime - (float)k) <= 0.0001f)
		{
			aiVectorKey key = srcNode->mPositionKeys[iPositionIndex];
			frameData.time = (float)key.mTime;
			::memcpy_s(&frameData.translation, sizeof(Vec3), &key.mValue, sizeof(aiVector3D));
			++iPositionIndex;
		}
		else if (::fabsf((float)srcNode->mPositionKeys[iPositionIndex].mTime - (float)k) >= 0.0001f && iPositionIndex < srcNode->mNumPositionKeys)
		{
			float fRatio = 0.0f;
			Vec3 vSourPosition, vDestPosition, vResultPosition;


			memcpy(&vSourPosition, &srcNode->mPositionKeys[iPositionIndex - 1].mValue, sizeof(Vec3));
			memcpy(&vDestPosition, &srcNode->mPositionKeys[iPositionIndex].mValue, sizeof(Vec3));
			
			fRatio = ((float)k - srcNode->mPositionKeys[iPositionIndex - 1].mTime) / (srcNode->mPositionKeys[iPositionIndex].mTime - srcNode->mPositionKeys[iPositionIndex - 1].mTime);

			vResultPosition = XMVectorLerp(vSourPosition, vDestPosition, fRatio);
			frameData.translation = vResultPosition;
		}


		// Rotation
		if (::fabsf((float)srcNode->mRotationKeys[iRotationIndex].mTime - (float)k) <= 0.0001f)
		{
			aiQuatKey key = srcNode->mRotationKeys[iRotationIndex];
			frameData.time = (float)key.mTime;

			frameData.rotation.x = key.mValue.x;
			frameData.rotation.y = key.mValue.y;
			frameData.rotation.z = key.mValue.z;
			frameData.rotation.w = key.mValue.w;

			++iRotationIndex;
		}
		else if (::fabsf((float)srcNode->mRotationKeys[iRotationIndex].mTime - (float)k) >= 0.0001f && iRotationIndex < srcNode->mNumRotationKeys)
		{
			float fRatio = 0.0f;
			Quaternion vSourRotation, vDestRotation, vResultRotation;

			vSourRotation.x = srcNode->mRotationKeys[iRotationIndex - 1].mValue.x;
			vSourRotation.y = srcNode->mRotationKeys[iRotationIndex - 1].mValue.y;
			vSourRotation.z = srcNode->mRotationKeys[iRotationIndex - 1].mValue.z;
			vSourRotation.w = srcNode->mRotationKeys[iRotationIndex - 1].mValue.w;

			vDestRotation.x = srcNode->mRotationKeys[iRotationIndex].mValue.x;
			vDestRotation.y = srcNode->mRotationKeys[iRotationIndex].mValue.y;
			vDestRotation.z = srcNode->mRotationKeys[iRotationIndex].mValue.z;
			vDestRotation.w = srcNode->mRotationKeys[iRotationIndex].mValue.w;
			

			fRatio = ((float)k - srcNode->mRotationKeys[iRotationIndex - 1].mTime) / (srcNode->mRotationKeys[iRotationIndex].mTime - srcNode->mRotationKeys[iRotationIndex - 1].mTime);


			vResultRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
			frameData.rotation = vResultRotation;
		}


		// Scale
		if (::fabsf((float)srcNode->mScalingKeys[iScaleIndex].mTime - (float)k) <= 0.0001f)
		{
			aiVectorKey key = srcNode->mScalingKeys[iScaleIndex];
			frameData.time = (float)key.mTime;
			::memcpy_s(&frameData.scale, sizeof(Vec3), &key.mValue, sizeof(aiVector3D));

			++iScaleIndex;
		}
		else if (::fabsf((float)srcNode->mScalingKeys[iScaleIndex].mTime - (float)k) >= 0.0001f && iScaleIndex < srcNode->mNumScalingKeys)
		{
			float fRatio = 0.0f;
			Vec3 vSourScale, vDestScale, vResultScale;


			memcpy(&vSourScale, &srcNode->mScalingKeys[iScaleIndex - 1].mValue, sizeof(Vec3));
			memcpy(&vDestScale, &srcNode->mScalingKeys[iScaleIndex].mValue, sizeof(Vec3));

			fRatio = ((float)k - srcNode->mScalingKeys[iScaleIndex - 1].mTime) / (srcNode->mScalingKeys[iScaleIndex].mTime - srcNode->mScalingKeys[iScaleIndex - 1].mTime);

			vResultScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
			frameData.scale = vResultScale;
		}

		node->keyframe.push_back(frameData);
	}


	return node;
}

void CModelConverter::Read_KeyframeData(shared_ptr<asAnimation> animation, aiNode* srcNode, map<string, shared_ptr<asAnimationNode>>& cache)
{
	shared_ptr<asKeyframe> keyframe = make_shared<asKeyframe>();
	keyframe->boneName = srcNode->mName.C_Str();

	shared_ptr<asAnimationNode> findNode = cache[srcNode->mName.C_Str()];

	for (uint32 i = 0; i < animation->frameCount; i++)
	{
		asKeyframeData frameData;

		if (findNode == nullptr)
		{
			Matrix transform(srcNode->mTransformation[0]);
			transform = transform.Transpose();
			frameData.time = (float)i;
			transform.Decompose(OUT frameData.scale, OUT frameData.rotation, OUT frameData.translation);
		}
		else
		{
			frameData = findNode->keyframe[i];
		}

		keyframe->transforms.push_back(frameData);
	}

	// 애니메이션 키프레임 채우기
	animation->keyframes.push_back(keyframe);

	for (uint32 i = 0; i < srcNode->mNumChildren; i++)
		Read_KeyframeData(animation, srcNode->mChildren[i], cache);
}

void CModelConverter::Write_AnimationData(wstring finalPath)
{
	auto path = filesystem::path(finalPath);

	// 폴더가 없으면 만든다.
	filesystem::create_directory(path.parent_path());

	shared_ptr<CAsFileUtils> file = make_shared<CAsFileUtils>();
	file->Open(finalPath, FileMode::Write);
	file->Write<uint32>(_animations.size());

	for (auto& iter : _animations)
	{
		shared_ptr<asAnimation> animation = iter;
		file->Write<string>(animation->name);
		file->Write<float>(animation->duration);
		file->Write<float>(animation->frameRate);
		file->Write<uint32>(animation->frameCount);

		file->Write<uint32>(animation->keyframes.size());

		for (shared_ptr<asKeyframe> keyframe : animation->keyframes)
		{
			file->Write<string>(keyframe->boneName);

			file->Write<uint32>(keyframe->transforms.size());
			file->Write(&keyframe->transforms[0], sizeof(asKeyframeData) * keyframe->transforms.size());
		}
	}

}

HRESULT CModelConverter::Set_BoneOffsetMatrix()
{
	for (_uint i = 0; i < _meshes.size(); ++i)
	{
		aiMesh* pMesh = _meshes[i]->mesh;

		for (_uint k = 0; k < pMesh->mNumBones; ++k)
		{
			aiBone* pAiBone = pMesh->mBones[k];

			_uint iIndex = -1;
			iIndex = GetBoneIndex(pAiBone->mName.C_Str());

			if (iIndex == -1)
				return E_FAIL;

			shared_ptr<asBone> pBone = _bones[iIndex];
			

			Matrix matOffset;

			memcpy(&matOffset, &pAiBone->mOffsetMatrix, sizeof(Matrix));

			matOffset = matOffset.Transpose();

			pBone->transform = matOffset.Invert();
		}
	}

	return S_OK;
}

_uint CModelConverter::GetBoneIndex(const string& name)
{
	for (shared_ptr<asBone>& bone : _bones)
	{
		if (bone->name == name)
			return bone->index;
	}

	assert(false);
	return 0;
}

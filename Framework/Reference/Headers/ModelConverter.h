#pragma once
#include "AsTypes.h"

BEGIN(Engine)

class CModelConverter
{
public:
	CModelConverter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFileName);
	~CModelConverter();

public:
	void Read_AssetFile(wstring file);
	void Set_aiScene(const aiScene* _aiScene) { _scene = _aiScene; }

	void Export_ModelData(wstring savePath);
	void Export_MaterialData(wstring savePath);
	void Export_AnimationData(wstring savePath);

	void Import_ModelData();
	void Import_MaterialData();
	void Import_AnimationData();


public:
	vector<shared_ptr<asBone>>* Get_Bones() { return &_bones; }
	vector<shared_ptr<asMesh>>*  Get_Meshes() { return &_meshes; }
	vector<shared_ptr<asMaterial>>*  Get_Materials() { return &_materials; }
	vector<shared_ptr<asAnimation>>*  Get_Animations() { return &_animations; }

	
	
	HRESULT Set_AnimationName(const wstring& strNewName, _uint iIndex);
	HRESULT Swap_Animation(_uint& iSrc, _uint& iDest);
	HRESULT DeleteAnimation(const wstring& strAnimation);

private:
	void Read_MaterialData();
	void Read_ModelData(aiNode* node, _uint index, int32 parent);
	void Read_MeshData(aiNode* node, _uint bone);
	shared_ptr<asAnimation> Read_AnimationData(aiAnimation* srcAnimation);
	void Write_ModelFile(wstring finalPath);

	shared_ptr<asAnimation> Find_Animation(const wstring& strAnimation);
private:
	
	void Write_MaterialData(wstring finalPath);
	string Write_Texture(string saveFolder, string file);

private:
	void Read_SkinData();
	shared_ptr<asAnimationNode> Parse_AnimationNode(shared_ptr<asAnimation> animation, aiNodeAnim* srcNode);
	void Read_KeyframeData(shared_ptr<asAnimation> animation, aiNode* srcNode, map<string, shared_ptr<asAnimationNode>>& cache);
	void Write_AnimationData(wstring finalPath);

	HRESULT Set_BoneOffsetMatrix();
private:
	_uint GetBoneIndex(const string& name);

private:
	wstring _assetPath = L"../Bin/Resources/Meshes/";
	wstring _modelPath = L"../Bin/Resources/Export/";
	wstring _texturePath = L"../Bin/Resources/Export/";
	wstring m_strFileName = L"Temp";

private:
	shared_ptr<Assimp::Importer> _importer;
	const aiScene* _scene;

private:
	vector<shared_ptr<asBone>> _bones;
	vector<shared_ptr<asMesh>> _meshes;
	vector<shared_ptr<asMaterial>> _materials;
	vector<shared_ptr<asAnimation>> _animations;

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
};

END
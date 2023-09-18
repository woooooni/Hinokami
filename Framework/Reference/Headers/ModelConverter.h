#pragma once
#include "AsTypes.h"

BEGIN(Engine)

class CModelConverter
{
public:
	CModelConverter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFileName);
	~CModelConverter();

public:
	void ReadAssetFile(wstring file);
	void Set_aiScene(const aiScene* _aiScene) { _scene = _aiScene; }

	void ExportModelData(wstring savePath);
	void ExportMaterialData(wstring savePath);
	void ExportAnimationData(wstring savePath);

	void ImportModelData();
	void ImportMaterialData();
	void ImportAnimationData();


public:
	vector<shared_ptr<asBone>>* Get_Bones() { return &_bones; }
	vector<shared_ptr<asMesh>>*  Get_Meshes() { return &_meshes; }
	vector<shared_ptr<asMaterial>>*  Get_Materials() { return &_materials; }
	map<const string, shared_ptr<asAnimation>>*  Get_Animations() { return &_animations; }

	
	
	HRESULT DeleteAnimation(const wstring& strAnimation);

private:
	void ReadMaterialData();
	void ReadModelData(aiNode* node, _uint index, int32 parent);
	void ReadMeshData(aiNode* node, _uint bone);
	shared_ptr<asAnimation> ReadAnimationData(aiAnimation* srcAnimation);
	void WriteModelFile(wstring finalPath);

	shared_ptr<asAnimation> Find_Animation(const wstring& strAnimation);
private:
	
	void WriteMaterialData(wstring finalPath);
	string WriteTexture(string saveFolder, string file);

private:
	void ReadSkinData();
	shared_ptr<asAnimationNode> ParseAnimationNode(shared_ptr<asAnimation> animation, aiNodeAnim* srcNode);
	void ReadKeyframeData(shared_ptr<asAnimation> animation, aiNode* srcNode, map<string, shared_ptr<asAnimationNode>>& cache);
	void WriteAnimationData(wstring finalPath);

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
	map<const string ,shared_ptr<asAnimation>> _animations;

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
};

END
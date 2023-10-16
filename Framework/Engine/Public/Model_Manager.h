#pragma once

#include "Base.h"

BEGIN(Engine)

class CModel_Manager final : public CBase
{
	DECLARE_SINGLETON(CModel_Manager);
public:
	CModel_Manager();
	virtual ~CModel_Manager() = default;

public:
	HRESULT Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	HRESULT Ready_Model_Data_FromPath(_uint iLevelIndex, _uint eType, const wstring& strFolderPath);
	
public:
	HRESULT Export_Model_Data(class CModel* pModel, const wstring& strSubFolderName, const wstring& strFileName);
	HRESULT Export_Model_Data_FromPath(_uint eType, wstring strFolderPath);
	HRESULT Import_Model_Data(_uint iLevelIndex, const wstring& strProtoTypeTag, _uint eType, wstring strFolderPath, wstring strFileName, __out class CModel** ppOut = nullptr);
	
	

private:
	HRESULT Export_Mesh(const wstring& strFinalPath, class CModel* pModel);
	HRESULT Export_Material(const wstring& strFinalPath, class CModel* pModel);
	HRESULT Export_Animation(const wstring& strFinalPath, class CModel* pModel);
	string Export_Texture(const wstring& strOriginFolder, const string& strSaveFolder, class CTexture* pTexture, _uint iIdx = 0);


private:
	HRESULT Import_Mesh(const wstring strFinalPath, class CModel* pModel);
	HRESULT Import_Material(const wstring strFinalPath, const wstring strFolderPath, class CModel* pModel);
	HRESULT Import_Animation(const wstring strFinalPath, class CModel* pModel);
	HRESULT Import_Texture(const wstring strFinalPath, class CModel* pModel);

private:
	wstring m_strExportFolderPath = L"../Bin/Export/";

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	_float4x4 m_PivotMatrix;

public:
	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 1. ������������ ���� �ε� �۾��� ���٤�������. */

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual~CLoader() = default;

public:
	const wstring Get_LoadingText() const {
		return m_strLoading;
	}

	_bool Get_Finished() const {
		return m_isFinished;
	}

public:
	/* eNextLevel�� ���� �ε��� �ϱ����� �����带 �����Ѵ�. */
	HRESULT Initialize(LEVELID eNextLevel, const wstring& strFolderName);
	_int	Loading();


private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	LEVELID					m_eNextLevel = { LEVEL_END };
	wstring					m_strFolderName = { L"" };
	wstring					m_strLoading = { TEXT("") };
	_bool					m_isFinished = { false };

private:
	HANDLE					m_hThread = { 0 };
	CRITICAL_SECTION		m_Critical_Section;



private:
	HRESULT Loading_For_Level_Logo();
	HRESULT Loading_For_Level_Train_Station();
	HRESULT Loading_For_Level_Train();
	HRESULT Loading_For_Level_Train_Boss();
	HRESULT Loading_For_Level_Final_Boss();
	HRESULT Loading_For_Level_Tool();
	HRESULT Load_Navi_Data(const wstring& strNaviFileName);
	HRESULT Load_Map_Data(const wstring& strMapFileName);

private:
	// ������ ����� ��� fbx ���� ��ü�� �ε��Ѵ�.
	HRESULT Loading_Proto_AllObjects(const wstring& strPath);

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel, const wstring& strFolderName);
	virtual void Free() override;
};

END 
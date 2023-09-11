#include "stdafx.h"
#include "..\Public\Data_Manager.h"

USING(Client)
IMPLEMENT_SINGLETON(CData_Manager)

CData_Manager::CData_Manager()
{
}


HRESULT CData_Manager::Reseve_Manager(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_hWnd = hWnd;

	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	
	return S_OK;
}

void CData_Manager::Save_ModelData(const wstring& strModelPath, const wstring& strModelFileName)
{
}

void CData_Manager::Load_ModelData(const wstring& strModelPath, const wstring& strModelFileName)
{
}

void CData_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

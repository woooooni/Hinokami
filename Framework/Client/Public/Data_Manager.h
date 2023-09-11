#pragma once
#include "Base.h"

BEGIN(Client)
class CData_Manager : public CBase
{
	DECLARE_SINGLETON(CData_Manager);

private:
	CData_Manager();
	virtual ~CData_Manager() = default;

public:
	HRESULT Reseve_Manager(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	void Save_ModelData(const wstring& strModelPath, const wstring& strModelFileName);
	void Load_ModelData(const wstring& strModelPath, const wstring &strModelFileName);
private:
	HWND m_hWnd;
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
public:
	virtual void Free() override;
};
END


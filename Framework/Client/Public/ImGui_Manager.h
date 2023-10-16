#pragma once

#include "Client_Defines.h"
#include "Engine_Macro.h"

#include "Base.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#pragma region IMGUI_MACRO

#define IMGUI_NEW_LINE ImGui::NewLine()
#define IMGUI_SAME_LINE ImGui::SameLine()
#pragma endregion


BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CImGui_Manager : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)

private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Reserve_Manager(HWND hWnd,  ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Tick(_float fTimeDelta);
	void	Render_ImGui();
	
	
public:
	void Set_Target(CGameObject* pObj)
	{
		if (nullptr == pObj)
			return;
		m_pTarget = pObj;
	}

	void Set_Dummy(class CDummy* pDummy) 
	{
		if (nullptr == pDummy)
			return;
		m_pDummy = pDummy;
	}

	void Set_Terrain(class CTerrain* pTerrain)
	{
		if (nullptr == pTerrain)
			return;
		m_pTerrain = pTerrain;
	}

	void Set_Camera(class CCamera_Free* pCamera)
	{
		if (nullptr == pCamera)
			return;

		m_pCamera = pCamera;
	}

private:
	void Tick_Basic_Tool(_float fTimeDelta);
	void Tick_Camera_Tool(_float fTimeDelta);
	void Tick_Hierachy(_float fTimeDelta);
	void Tick_Inspector(_float fTimeDelta);
	void Tick_Model_Tool(_float fTimeDelta);
	void Tick_Animation_Tool(_float fTimeDelta);
	void Tick_Effect_Tool(_float fTimeDelta);
	void Tick_Map_Tool(_float fTimeDelta);
	void Tick_Terrain_Tool(_float fTimeDelta);
	
private:
	HRESULT Load_Map_Data(const wstring& strMapFileName);
	HRESULT Save_Map_Data(const wstring& strMapFileName);

private:
	void PickingGroundObj();
	void PickingTerrainObj();

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	HWND m_hWnd = nullptr;

private:
	_bool m_bShowDemo = true;
	_bool m_bShowModelWindow = false;
	_bool m_bShowEffectWindow = false;
	_bool m_bShowMapWindow = true;
	_bool m_bShowTerrainWindow = true;

	_float m_fWindowAlpha = 1.f;

	wstring m_strFilePath = TEXT("../Bin/Export/Zenitsu/");
	wstring m_strFileName = TEXT("Zenitsu");
	wstring m_strExportPath = TEXT("../Bin/Resources/Export/");

public:
	class CGameObject* m_pTarget = nullptr;
	class CGameObject* m_pPrevObject = nullptr;
	class CMesh_Effect* m_pPrevMeshEffect = nullptr;
	class CTexture_Effect* m_pPrevTextureEffect = nullptr;

	class CDummy* m_pDummy = nullptr;
	class CTerrain* m_pTerrain = nullptr;
	class CCamera_Free* m_pCamera = nullptr;

public:
	virtual void Free() override;
};
END
#pragma once

#include "Client_Defines.h"
#include "Engine_Macro.h"

#include "Base.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "Terrain.h"
#include "Camera_Free.h"
#include "Dummy.h"
#include "GameObject.h"

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

public:
	typedef struct tagModelExportDesc
	{
		wstring strModelPath;
		_uint iModelType;
	} MODEL_EXPORT_DESC;


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
	void Tick_Particle_Tool(_float fTimeDelta);
	void Tick_Map_Tool(_float fTimeDelta);
	void Tick_Terrain_Tool(_float fTimeDelta);
	void Tick_NaviMesh_Tool(_float fTimeDeleta);
	
private:
	HRESULT Load_Map_Data(const wstring& strMapFileName);
	HRESULT Save_Map_Data(const wstring& strMapFileName);

private:
	HRESULT Save_Effect(const wstring& strFullPath);
	HRESULT Load_Effect(const wstring& strFullPath);
	HRESULT Load_EffectsModelPath(const wstring& strEffectPath);


	HRESULT Save_Particle(const wstring& strFullPath);
	HRESULT Load_Particle(const wstring& strFullPath);

private:
	void PickingGroundObj();
	void PickingTerrainObj();

private:
	void Draw_NaviPicking_Point();
	void NaviPicking();
	HRESULT NaviAutoGenerate();
	HRESULT NaviBake();
	HRESULT NaviSave(const wstring& strNaviName);
	HRESULT NaviLoad(const wstring& strNaviName);
	

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	HWND m_hWnd = nullptr;


private:
	_bool m_bShowDemo = true;
	_bool m_bShowModelWindow = false;
	_bool m_bShowEffectWindow = false;
	_bool m_bShowParticleWindow = false;
	_bool m_bShowMapWindow = true;
	_bool m_bShowTerrainWindow = true;
	_bool m_bShowNavigationWindow = false;

	_float m_fWindowAlpha = 1.f;

	wstring m_strFilePath = TEXT("../Bin/Export/Zenitsu/");
	wstring m_strFileName = TEXT("Zenitsu");
	wstring m_strExportPath = TEXT("../Bin/Resources/Export/");


private:
	// For.Navigation
	class PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	class BasicEffect* m_pEffect = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;

	_float4	m_vColor = _float4(0.f, 1.f, 0.f, 1.f);
	_float3 m_vNaviPickingWorldPos = _float3(0.f, 0.f, 0.f);

	_float m_fGenerateRadian = 0.f;
	_float m_fGenerateMinScale = 1.f;
	_float m_fGenerateMaxScale = 100.f;

	vector<wstring> m_strBakeableObject;
	vector<_float3> m_vWorldPickedNaviPos;

	_float m_fMinTriangleY = 0.f;
	_float m_fMaxTriangleY = 10.f;



private:
	// For.Effect Texture
	vector<wstring> m_EffectsModelFiles;

public:
	class CGameObject* m_pTarget = nullptr;
	class CGameObject* m_pPrevObject = nullptr;


	class CEffect* m_pPrevEffect = nullptr;
	class CParticle* m_pPrevParticle = nullptr;

	class CDummy* m_pDummy = nullptr;
	class CTerrain* m_pTerrain = nullptr;
	class CCamera_Free* m_pCamera = nullptr;


public:
	virtual void Free() override;
};
END
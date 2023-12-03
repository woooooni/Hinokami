#include "stdafx.h"
#include "Level_Tool.h"
#include "GameInstance.h"
#include "Camera.h"
#include "ImGui_Manager.h"
#include "Player.h"
#include "Character.h"
#include "Dummy.h"
#include "Terrain.h"
#include "Camera_Manager.h"

#include "Network_Manager.h"


CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()
{
	SetWindowText(g_hWnd, TEXT("Scene_Tool"));
	if (FAILED(__super::Initialize()))
		return E_FAIL;

 	m_pImGuiManager = CImGui_Manager::GetInstance();
	Safe_AddRef(m_pImGuiManager);

	if (FAILED(Ready_Layer_Camera(LAYER_TYPE::LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(LAYER_TYPE::LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(LAYER_TYPE::LAYER_TERRAIN)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(LAYER_TYPE::LAYER_PLAYER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Character(LAYER_TYPE::LAYER_CHARACTER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(LAYER_TYPE::LAYER_MONSTER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(LAYER_TYPE::LAYER_EFFECT)))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_TYPE::LAYER_UI)))
		return E_FAIL;


	CGameObject* pObject = GAME_INSTANCE->Find_GameObject(LEVEL_TOOL, _uint(LAYER_TYPE::LAYER_PLAYER), L"Dummy");
	if (nullptr == pObject)
		return S_OK;

	CDummy* pDummy = dynamic_cast<CDummy*>(pObject);
	if (nullptr == pDummy)
		return S_OK;

	m_pImGuiManager->Set_Dummy(pDummy);


	CGameObject* pTempObject = GAME_INSTANCE->Find_GameObject(LEVEL_TOOL, _uint(LAYER_TYPE::LAYER_TERRAIN), L"Terrain");
	if (nullptr == pTempObject)
		return E_FAIL;

	CTerrain* pTerrain = dynamic_cast<CTerrain*>(pTempObject);
	if (nullptr == pTerrain)
		return E_FAIL;


	m_pImGuiManager->Set_Terrain(pTerrain);
	m_pImGuiManager->Set_Camera(dynamic_cast<CCamera_Free*>(CCamera_Manager::GetInstance()->Get_MainCamera()));




	return S_OK;
}

HRESULT CLevel_Tool::Tick(_float fTimeDelta)
{
	m_pImGuiManager->Tick(fTimeDelta);
	return S_OK;
}

HRESULT CLevel_Tool::LateTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_Tool::Render_Debug()
{
	m_pImGuiManager->Render_ImGui();
	return S_OK;
}

HRESULT CLevel_Tool::Enter_Level()
{
	/*Protocol::S_ENTER_LEVEL tSendPkt;
	tSendPkt.set_ilevelid(LEVEL_TOOL);
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(tSendPkt);
	CNetwork_Manager::GetInstance()->Send(sendBuffer);*/

	return S_OK;
}

HRESULT CLevel_Tool::Exit_Level()
{
	return S_OK;
}

HRESULT CLevel_Tool::Ready_Lights()
{
	


	//GAME_INSTANCE->Reset_Lights();

	//LIGHTDESC			LightDesc;

	//ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	//LightDesc.eType = LIGHTDESC::TYPE_POINT;
	//LightDesc.vPosition = _float4(15.0f, 5.0f, 15.0f, 1.f);
	//LightDesc.fRange = 10.f;
	//LightDesc.vDiffuse = _float4(1.f, 0.0f, 0.f, 1.f);
	//LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	//LightDesc.vSpecular = LightDesc.vDiffuse;

	//if (FAILED(GAME_INSTANCE->Add_Light(m_pDevice, m_pContext, LightDesc)))
	//	return E_FAIL;

	//ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	//LightDesc.eType = LIGHTDESC::TYPE_POINT;
	//LightDesc.vPosition = _float4(25.0f, 5.0f, 15.0f, 1.f);
	//LightDesc.fRange = 10.f;
	//LightDesc.vDiffuse = _float4(0.0f, 1.f, 0.f, 1.f);
	//LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	//LightDesc.vSpecular = LightDesc.vDiffuse;

	//if (FAILED(GAME_INSTANCE->Add_Light(m_pDevice, m_pContext, LightDesc)))
	//	return E_FAIL;

	//ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	//LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	//LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	//LightDesc.vDiffuse = _float4(0.5, 0.5, 0.5, 1.f);
	//LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	//if (FAILED(GAME_INSTANCE->Add_Light(m_pDevice, m_pContext, LightDesc)))
	//	return E_FAIL;



	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Camera(const LAYER_TYPE eLayerType)
{
	CCamera_Manager::GetInstance()->Set_MainCamera(CCamera_Manager::CAMERA_TYPE::TOOL);


	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Player(const LAYER_TYPE eLayerType)
{
	if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_TOOL, _uint(eLayerType), TEXT("Prototype_GameObject_Dummy"))))
		return E_FAIL;



	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Character(const LAYER_TYPE eLayerType)
{
	/*if (FAILED(GI->Add_GameObject(LEVEL_TOOL, _uint(eLayerType), TEXT("Prototype_GameObject_Zenitsu"))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_BackGround(const LAYER_TYPE eLayerType)
{

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Terrain(const LAYER_TYPE eLayerType)
{
	if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_TOOL, LAYER_TYPE::LAYER_TERRAIN, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Monster(const LAYER_TYPE eLayerType)
{
	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_UI(const LAYER_TYPE eLayerType)
{
	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Effect(const LAYER_TYPE eLayerType)
{

	return S_OK;
}

CLevel_Tool* CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tool* pInstance = new CLevel_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Tool::Free()
{
	__super::Free();
	Safe_Release(m_pImGuiManager);


}

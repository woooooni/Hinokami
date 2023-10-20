#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Camera_Main.h"
#include "Character.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_TYPE::LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(LAYER_TYPE::LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(LAYER_TYPE::LAYER_PLAYER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(LAYER_TYPE::LAYER_MONSTER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(LAYER_TYPE::LAYER_EFFECT)))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_TYPE::LAYER_UI)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_GamePlay::LateTick(_float fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("게임플레이 레벨입니다."));
	return S_OK;
}

HRESULT CLevel_GamePlay::Enter_Level()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Exit_Level()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	


	LIGHTDESC			LightDesc;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = _float4(15.0f, 5.0f, 15.0f, 1.f);
	LightDesc.fRange = 10.f;
	LightDesc.vDiffuse = _float4(1.f, 0.0f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(GAME_INSTANCE->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = _float4(25.0f, 5.0f, 15.0f, 1.f);
	LightDesc.fRange = 10.f;
	LightDesc.vDiffuse = _float4(0.0f, 1.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(GAME_INSTANCE->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.5, 0.5, 0.5, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(GAME_INSTANCE->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const LAYER_TYPE eLayerType)
{
	


	CCamera::CAMERADESC			CameraDesc;

	CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.0f;

	CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

 	if(FAILED(GI->Add_GameObject(LEVELID::LEVEL_GAMEPLAY, LAYER_CAMERA, TEXT("Prototype_GameObject_Camera_Main"), &CameraDesc)))
		return E_FAIL;

	/*if (FAILED(GI->Add_GameObject(LEVELID::LEVEL_GAMEPLAY, LAYER_CAMERA, TEXT("Prototype_GameObject_Camera_Main"), &CameraDesc)))
		return E_FAIL;*/
	
	GI->Lock_Mouse();
	

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const LAYER_TYPE eLayerType)
{
	CGameObject* pTanjiro = nullptr;
	if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_GAMEPLAY, LAYER_TYPE::LAYER_CHARACTER, TEXT("Prototype_GameObject_Tanjiro"), nullptr, &pTanjiro)))
		return E_FAIL;

	CGameObject* pObject = GI->Find_GameObejct(LEVELID::LEVEL_GAMEPLAY, LAYER_CAMERA, L"Main_Camera");
	if (nullptr == pObject)
		return E_FAIL;

	CCamera_Main* pCamera = dynamic_cast<CCamera_Main*>(pObject);
	if (nullptr == pCamera)
		return E_FAIL;

	CCharacter* pCharacter = dynamic_cast<CCharacter*>(pTanjiro);
	if (nullptr == pCharacter)
		return E_FAIL;

	if(FAILED(pCamera->Set_TargetTransform(pCharacter->Get_TransformCom())))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Character(const LAYER_TYPE eLayerType)
{
	return E_NOTIMPL;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const LAYER_TYPE eLayerType)
{
	


	/*if (FAILED(GAME_INSTANCE->Add_GameObject(TEXT("Prototype_GameObject_Terrain"), LEVEL_GAMEPLAY, _uint(eLayerType))))
		return E_FAIL;*/

	;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const LAYER_TYPE eLayerType)
{
	

	

	;
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const LAYER_TYPE eLayerType)
{
	


	/*for (_uint i = 0; i < 1; ++i)
	{
		if (FAILED(GAME_INSTANCE->Add_GameObject(TEXT("Prototype_GameObject_UI"), LEVEL_GAMEPLAY, _uint(eLayerType))))
			return E_FAIL;

	} */

	;



	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const LAYER_TYPE eLayerType)
{
	



	;


	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_GamePlay*	pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();


}

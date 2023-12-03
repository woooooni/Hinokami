#include "stdafx.h"
#include "Camera_Manager.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Camera_Main.h"

IMPLEMENT_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
{

}

HRESULT CCamera_Manager::Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	if (FAILED(Ready_Cameras()))
		return E_FAIL;

	return S_OK;
}

void CCamera_Manager::Tick(_float fTimeDelta)
{
	if(nullptr != m_pMainCamera)
		m_pMainCamera->Tick(fTimeDelta);
}

void CCamera_Manager::LateTick(_float fTimeDelta)
{
	if (nullptr != m_pMainCamera)
		m_pMainCamera->LateTick(fTimeDelta);
}

HRESULT CCamera_Manager::Ready_Cameras()
{
	CCamera::CAMERADESC			CameraDesc;

	CameraDesc.vEye = _float4(0.f, 30.f, -30.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 1000.f;

	CameraDesc.TransformDesc.fSpeedPerSec = 3.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_pCameras[CAMERA_TYPE::TOOL] = CCamera_Free::Create(m_pDevice, m_pContext, L"Free_Camera");
	if (nullptr == m_pCameras[CAMERA_TYPE::TOOL])
		return E_FAIL;
	if (FAILED(m_pCameras[CAMERA_TYPE::TOOL]->Initialize(&CameraDesc)))
	{
		Safe_Release(m_pCameras[CAMERA_TYPE::TOOL]);
		return E_FAIL;
	}




	m_pCameras[CAMERA_TYPE::GAME_PLAY] = CCamera_Main::Create(m_pDevice, m_pContext, L"Main_Camera");
	if (nullptr == m_pCameras[CAMERA_TYPE::GAME_PLAY])
		return E_FAIL;

	if (FAILED(m_pCameras[CAMERA_TYPE::GAME_PLAY]->Initialize(&CameraDesc)))
	{
		Safe_Release(m_pCameras[CAMERA_TYPE::GAME_PLAY]);
		return E_FAIL;
	}

	
	return S_OK;
}

void CCamera_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}



#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

CMainApp::CMainApp()	
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT CMainApp::Initialize()
{
	/* 1. �� ������ �ʱ�ȭ�� �����Ҳ���. */
	/* 1-1. �׷�����ġ�� �ʱ�ȭ�Ѵ�. */
	/* 1-2. ������ġ�� �ʱ�ȭ�Ѵ�. */
	/* 1-3. �Է���ġ�� �ʱ�ȭ�Ѵ�. */
	GRAPHIC_DESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.eWinMode = GRAPHIC_DESC::WINMODE_WIN;
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	/* 1-4. ���ӳ����� ����� ����(��)�� �����Ѵ�.   */
	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;


	/* 1-4-1. ���ӳ����� ����� ���� �ڿ�(�ؽ���, ��, ��ü) ���� �غ��Ѵ�.  */

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	/* ���ӳ��� �����ϴ� ���� ��ü���� ����. */
	/* ������ ���� */
	m_pGameInstance->Tick(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	/* ���ӳ��� �����ϴ� ���� ��ü���� ������. */
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pGameInstance->Draw();

	/* �ʱ�ȭ�� ��鿡 ��ü���� �׸���. */
	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* �ΰ��������� �Ҵ��ϰ� �;�����. �ΰ������� ���� �ε������� ���� �����Ͽ� �ε��۾��� �����Ҳ���. */
	/* �ε���ü���� eLevelID��� ���� ���� �Ҵ社�ư� �;��� ��������ü�� �ذ���?! �����Ҵ��ϰ��;��� ������ �ڿ��� �غ���϶�� */
	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::CMainApp::Free()
{	
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
	

}
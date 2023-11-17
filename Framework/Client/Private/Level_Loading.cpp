#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#include "GameInstance.h"
#include "Loader.h"

#include "Level_Logo.h"
#include "Level_Train.h"
#include "Level_Train_Station.h"
#include "Level_Train_Boss.h"
#include "Level_FinalBoss.h"

#include "Level_Tool.h"
#include "ImGui_Manager.h"
#include "Network_Manager.h"


CLevel_Loading::CLevel_Loading(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}


HRESULT CLevel_Loading::Initialize(LEVELID eNextLevel, const wstring& strFolderName)
{
	m_eNextLevel = eNextLevel;

	if (m_eNextLevel == LEVEL_TOOL)
	{
		CImGui_Manager::GetInstance()->Reserve_Manager(g_hWnd, m_pDevice, m_pContext);
	}

	if(FAILED(Ready_LoadingUI()))
		return E_FAIL;

	/* m_eNextLevel 에 대한 로딩작업을 수행한다. */
	/* 로딩을 겁나 하고있다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevel, strFolderName);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_Loading::LateTick(_float fTimeDelta)
{
	wstring		strLoadingText = m_pLoader->Get_LoadingText();

	SetWindowText(g_hWnd, strLoadingText.c_str());

	if (true == m_pLoader->Get_Finished())
	{
		if (KEY_TAP(KEY::SPACE))
		{
			CLevel* pNewLevel = nullptr;

			switch (m_eNextLevel)
			{
			case LEVEL_LOGO:
				pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;

			case LEVEL_TRAIN_STATION:
				pNewLevel = CLevel_Train_Station::Create(m_pDevice, m_pContext);
				break;

			case LEVEL_TRAIN:
				pNewLevel = CLevel_Train::Create(m_pDevice, m_pContext);
				break;

			case LEVEL_TRAIN_BOSS:
				pNewLevel = CLevel_Train_Boss::Create(m_pDevice, m_pContext);
				break;

			case LEVEL_FINAL_BOSS:
				pNewLevel = CLevel_FinalBoss::Create(m_pDevice, m_pContext);
				break;

			case LEVEL_TOOL:
				pNewLevel = CLevel_Tool::Create(m_pDevice, m_pContext);
				break;
			}

			if (nullptr == pNewLevel)
				return E_FAIL;

			if (FAILED(GAME_INSTANCE->Open_Level(m_eNextLevel, pNewLevel)))
				return E_FAIL;
		}

	}


	return S_OK;
}

HRESULT CLevel_Loading::Enter_Level()
{
	return S_OK;
}

HRESULT CLevel_Loading::Exit_Level()
{
	return S_OK;
}

HRESULT CLevel_Loading::Ready_LoadingUI()
{
	
	if (FAILED(GI->Add_GameObject(LEVEL_LOADING, LAYER_TYPE::LAYER_UI, L"Prototype_GameObject_UI_Loading_BackGround")))
		return E_FAIL;

	if (FAILED(GI->Add_GameObject(LEVEL_LOADING, LAYER_TYPE::LAYER_UI, L"Prototype_GameObject_UI_Loading_Anim")))
		return E_FAIL;

	if (FAILED(GI->Add_GameObject(LEVEL_LOADING, LAYER_TYPE::LAYER_UI, L"Prototype_GameObject_UI_Loading_Icon")))
		return E_FAIL;

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevel, const wstring& strFolderName)
{
	CLevel_Loading*	pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel, strFolderName)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}

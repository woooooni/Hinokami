#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "ServerSession.h"
#include "Service.h"
#include "ThreadManager.h"
#include "Network_Manager.h"

CLevel_Logo::CLevel_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

#pragma region Server Enter Code

	//SetWindowText(g_hWnd, TEXT("서버에 접속중입니다."));

	//ClientPacketHandler::Init();

	//this_thread::sleep_for(1s);

	//ClientServiceRef service = make_shared<ClientService>(
	//	NetAddress(SERVER_IP, SERVER_PORT),
	//	make_shared<IocpCore>(),
	//	make_shared<CServerSession>, // TODO : SessionManager 등
	//	1);

	//ASSERT_CRASH(service->Start());

	//for (int32 i = 0; i < 3; i++)
	//{
	//	ThreadManager::GetInstance()->Launch([=]()
	//		{
	//			while (true)
	//			{
	//				service->GetIocpCore()->Dispatch();
	//			}
	//		});
	//}


	//while (CNetwork_Manager::GetInstance()->Is_Connected() == false)
	//{
	//}
#pragma endregion
	return S_OK;
}

HRESULT CLevel_Logo::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_Logo::LateTick(_float fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

	//if (GetKeyState(VK_RETURN) & 0x8000)
	//{
	//	if (FAILED(GAME_INSTANCE->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
	//		return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_Logo::Enter_Level()
{
	return S_OK;
}

HRESULT CLevel_Logo::Exit_Level()
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround()
{
	/* 원형객체를 복제하여 사본객체를 생성하고 레이어에 추가한다. */
	if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_LOGO, LAYER_TYPE::LAYER_UI, L"Prototype_GameObject_UI_Logo_BackGround")))
		return E_FAIL;


	if(FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_LOGO, LAYER_TYPE::LAYER_UI,L"Prototype_GameObject_UI_Logo_Title")))
		return E_FAIL;
	
	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Logo*	pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
 	__super::Free();


}

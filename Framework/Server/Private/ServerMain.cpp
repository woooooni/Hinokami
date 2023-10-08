#include "stdafx.h"
#include "ServerMain.h"
#include "GameInstance.h"
#include "SocketUtils.h"
#include "GameSessionManager.h"
#include "Service.h"
#include "ThreadManager.h"
#include "ServerObject_Manager.h"
#include "Utils.h"

CServerMain::CServerMain()
{
}

HRESULT CServerMain::Initialize()
{
	SocketUtils::Init();
	ServerPacketHandler::Init();

	ServerServiceRef service = std::make_shared<ServerService>(
		NetAddress(L"192.168.1.33", 7777),
		std::make_shared<IocpCore>(),
		std::make_shared<CGameSession>, // TODO : SessionManager µî
		10);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		ThreadManager::GetInstance()->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	if (FAILED(CServerObject_Manager::GetInstance()->Reserve_Manager()))
		return E_FAIL;



	cout << "Server Open" << endl;


	
	return S_OK;
}

void CServerMain::Start_Server()
{
	
}

void CServerMain::Tick_Server()
{

}

CServerMain* CServerMain::Create()
{
	CServerMain* pInstance = new CServerMain();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CServerMain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CServerMain::Free()
{
	ThreadManager::GetInstance()->Join();

}

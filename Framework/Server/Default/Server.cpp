#include "stdafx.h"
#include "ThreadManager.h"
#include "GameInstance.h"
#include "ServerMain.h"


int main()
{
	CServerMain* pMainApp = CServerMain::Create();

	if (nullptr == pMainApp)
	{
		MSG_BOX("Server Open Failed.");
		return 0;
	}

	pMainApp->Start_Server();

	while (!pMainApp->Is_ShutDown())
		pMainApp->Tick_Server();

	Safe_Release(pMainApp);

}
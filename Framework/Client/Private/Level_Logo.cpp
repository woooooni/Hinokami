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


	SetWindowText(g_hWnd, TEXT("������ �������Դϴ�."));

	ClientPacketHandler::Init();

	this_thread::sleep_for(1s);

	ClientServiceRef service = make_shared<ClientService>(
		NetAddress(SERVER_IP, SERVER_PORT),
		make_shared<IocpCore>(),
		make_shared<CServerSession>, // TODO : SessionManager ��
		1);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 3; i++)
	{
		ThreadManager::GetInstance()->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}


	while (CNetwork_Manager::GetInstance()->Is_Connected() == false)
	{
	}

	return S_OK;
}

HRESULT CLevel_Logo::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_Logo::LateTick(_float fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("�ΰ����Դϴ�."));

	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(GAME_INSTANCE->Open_Level(LEVEL_LOADING,  CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return E_FAIL;
	}



	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround()
{
	/* ������ü�� �����Ͽ� �纻��ü�� �����ϰ� ���̾ �߰��Ѵ�. */
	


	;

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

#include "stdafx.h"
#include "Level_Train_Station.h"
#include "GameInstance.h"

#include "Camera_Manager.h"
#include "Character.h"



CLevel_Train_Station::CLevel_Train_Station(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Train_Station::Initialize()
{
	GI->Lock_Mouse();
	GI->Stop_All();
	GI->Play_BGM(L"Train_Station.wav", 1.f);

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

	
	if (FAILED(Ready_Layer_Npc(LAYER_TYPE::LAYER_NPC)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(LAYER_TYPE::LAYER_EFFECT)))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_TYPE::LAYER_UI)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Train_Station::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_Train_Station::LateTick(_float fTimeDelta)
{
	
	return S_OK;
}

HRESULT CLevel_Train_Station::Enter_Level()
{
	return S_OK;
}

HRESULT CLevel_Train_Station::Exit_Level()
{
	return S_OK;
}

HRESULT CLevel_Train_Station::Ready_Lights()
{
	return S_OK;
}

HRESULT CLevel_Train_Station::Ready_Layer_Camera(const LAYER_TYPE eLayerType)
{
	CCamera_Manager::GetInstance()->Set_MainCamera(CCamera_Manager::CAMERA_TYPE::GAME_PLAY);

	if (FAILED(GI->Add_GameObject(LEVELID::LEVEL_TRAIN_STATION, LAYER_BACKGROUND, TEXT("Prototype_GameObject_Sky_Sunset"))))
		return E_FAIL;
	
	

	return S_OK;
}

HRESULT CLevel_Train_Station::Ready_Layer_Player(const LAYER_TYPE eLayerType)
{
	CGameObject* pTanjiro = nullptr;
	if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_TRAIN_STATION, LAYER_TYPE::LAYER_CHARACTER, TEXT("Prototype_GameObject_Tanjiro"), nullptr, &pTanjiro)))
		return E_FAIL;

	CCharacter* pCharacter = dynamic_cast<CCharacter*>(pTanjiro);
	if (nullptr == pCharacter)
		return E_FAIL;


	//CGameObject* pZenitsu = nullptr;
	//if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_TRAIN_STATION, LAYER_TYPE::LAYER_CHARACTER, TEXT("Prototype_GameObject_Zenitsu"), nullptr, &pZenitsu)))
	//	return E_FAIL;

	//CCharacter* pCharacter = dynamic_cast<CCharacter*>(pZenitsu);
	//if (nullptr == pCharacter)
	//	return E_FAIL;

	CNavigation* pNavigation = pCharacter->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr != pNavigation)
	{
		CNavigation::NAVIGATION_DESC NavigationDesc;
		NavigationDesc.bInitialize_Index = true;
		NavigationDesc.vStartWorldPosition = _float4(5.f, -1.5f, -74.f, 1.f);
		if (FAILED(pNavigation->Initialize(&NavigationDesc)))
			return E_FAIL;
		
		
		pCharacter->Get_Component<CTransform>(L"Com_Transform")->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pNavigation->Get_NaviDesc().vStartWorldPosition));
	}


	CCamera* pMainCamera = CCamera_Manager::GetInstance()->Get_MainCamera();
	if (FAILED(pMainCamera->Set_TargetTransform(pCharacter->Get_Component<CTransform>(L"Com_Transform"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Train_Station::Ready_Layer_Character(const LAYER_TYPE eLayerType)
{
	return S_OK;
}

HRESULT CLevel_Train_Station::Ready_Layer_Npc(const LAYER_TYPE eLayerType)
{
	CGameObject* pNpc = nullptr;
	if (FAILED(GI->Add_GameObject(LEVEL_TRAIN_STATION, LAYER_TYPE::LAYER_NPC, TEXT("Prototype_GameObject_Npc_Boy_0"), nullptr, &pNpc)))
		return E_FAIL;

	CNavigation* pNavigation = pNpc->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr != pNavigation)
	{
		CNavigation::NAVIGATION_DESC NaviDesc;
		NaviDesc.bInitialize_Index = true;
		NaviDesc.vStartWorldPosition = _float4(-7.5f, -1.9f, -51.f, 1.f);

		if (FAILED(pNavigation->Initialize(&NaviDesc)))
			return E_FAIL;

		
		CTransform* pTransform = pNpc->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr != pTransform)
		{
			pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
			pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pNavigation->Get_NaviDesc().vStartWorldPosition));
		}
	}


	if (FAILED(GI->Add_GameObject(LEVEL_TRAIN_STATION, LAYER_TYPE::LAYER_NPC, TEXT("Prototype_GameObject_Npc_Girl_0"), nullptr, &pNpc)))
		return E_FAIL;

	pNavigation = pNpc->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr != pNavigation)
	{
		CNavigation::NAVIGATION_DESC NaviDesc;
		NaviDesc.bInitialize_Index = true;
		NaviDesc.vStartWorldPosition = _float4(16.f, -1.9f, -45.8f, 1.f);

		if (FAILED(pNavigation->Initialize(&NaviDesc)))
			return E_FAIL;


		CTransform* pTransform = pNpc->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr != pTransform)
		{
			pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
			pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pNavigation->Get_NaviDesc().vStartWorldPosition));
		}
	}

	if (FAILED(GI->Add_GameObject(LEVEL_TRAIN_STATION, LAYER_TYPE::LAYER_NPC, TEXT("Prototype_GameObject_Npc_Boy_0"), nullptr, &pNpc)))
		return E_FAIL;

	pNavigation = pNpc->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr != pNavigation)
	{
		CNavigation::NAVIGATION_DESC NaviDesc;
		NaviDesc.bInitialize_Index = true;
		NaviDesc.vStartWorldPosition = _float4(-6.f, -1.9f, -24.f, 1.f);

		if (FAILED(pNavigation->Initialize(&NaviDesc)))
			return E_FAIL;


		CTransform* pTransform = pNpc->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr != pTransform)
		{
			pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
			pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pNavigation->Get_NaviDesc().vStartWorldPosition));
		}
	}

	if (FAILED(GI->Add_GameObject(LEVEL_TRAIN_STATION, LAYER_TYPE::LAYER_NPC, TEXT("Prototype_GameObject_Npc_Girl_1"), nullptr, &pNpc)))
		return E_FAIL;

	pNavigation = pNpc->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr != pNavigation)
	{
		CNavigation::NAVIGATION_DESC NaviDesc;
		NaviDesc.bInitialize_Index = true;
		NaviDesc.vStartWorldPosition = _float4(-6.f, -1.9f, -26.f, 1.f);

		if (FAILED(pNavigation->Initialize(&NaviDesc)))
			return E_FAIL;


		CTransform* pTransform = pNpc->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr != pTransform)
		{
			pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pNavigation->Get_NaviDesc().vStartWorldPosition));
		}
	}

	if (FAILED(GI->Add_GameObject(LEVEL_TRAIN_STATION, LAYER_TYPE::LAYER_NPC, TEXT("Prototype_GameObject_Npc_PoliceMan_0"), nullptr, &pNpc)))
		return E_FAIL;

	pNavigation = pNpc->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr != pNavigation)
	{
		CNavigation::NAVIGATION_DESC NaviDesc;
		NaviDesc.bInitialize_Index = true;
		NaviDesc.vStartWorldPosition = _float4(4.f, -1.9f, 1.4f, 1.f);

		if (FAILED(pNavigation->Initialize(&NaviDesc)))
			return E_FAIL;


		CTransform* pTransform = pNpc->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr != pTransform)
		{
			pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
			pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pNavigation->Get_NaviDesc().vStartWorldPosition));
		}
	}

	if (FAILED(GI->Add_GameObject(LEVEL_TRAIN_STATION, LAYER_TYPE::LAYER_NPC, TEXT("Prototype_GameObject_Npc_PoliceMan_0"), nullptr, &pNpc)))
		return E_FAIL;

	pNavigation = pNpc->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr != pNavigation)
	{
		CNavigation::NAVIGATION_DESC NaviDesc;
		NaviDesc.bInitialize_Index = true;
		NaviDesc.vStartWorldPosition = _float4(1.f, -1.9f, 1.4f, 1.f);

		if (FAILED(pNavigation->Initialize(&NaviDesc)))
			return E_FAIL;


		CTransform* pTransform = pNpc->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr != pTransform)
		{
			pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
			pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pNavigation->Get_NaviDesc().vStartWorldPosition));
		}
	}







	if (FAILED(GI->Add_GameObject(LEVEL_TRAIN_STATION, LAYER_TYPE::LAYER_NPC, TEXT("Prototype_GameObject_Npc_Boy_0"), nullptr, &pNpc)))
		return E_FAIL;

	pNavigation = pNpc->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr != pNavigation)
	{
		CNavigation::NAVIGATION_DESC NaviDesc;
		NaviDesc.bInitialize_Index = true;
		NaviDesc.vStartWorldPosition = _float4(43.5f, -0.5f, 33.8f, 1.f);

		if (FAILED(pNavigation->Initialize(&NaviDesc)))
			return E_FAIL;


		CTransform* pTransform = pNpc->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr != pTransform)
		{
			pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
			pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pNavigation->Get_NaviDesc().vStartWorldPosition));
		}
	}


	if (FAILED(GI->Add_GameObject(LEVEL_TRAIN_STATION, LAYER_TYPE::LAYER_NPC, TEXT("Prototype_GameObject_Npc_Girl_1"), nullptr, &pNpc)))
		return E_FAIL;

	pNavigation = pNpc->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr != pNavigation)
	{
		CNavigation::NAVIGATION_DESC NaviDesc;
		NaviDesc.bInitialize_Index = true;
		NaviDesc.vStartWorldPosition = _float4(44.5f, -0.5f, 33.8f, 1.f);

		if (FAILED(pNavigation->Initialize(&NaviDesc)))
			return E_FAIL;


		CTransform* pTransform = pNpc->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr != pTransform)
		{
			pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
			pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pNavigation->Get_NaviDesc().vStartWorldPosition));
		}
	}



	if (FAILED(GI->Add_GameObject(LEVEL_TRAIN_STATION, LAYER_TYPE::LAYER_NPC, TEXT("Prototype_GameObject_Npc_OldWoman_0"), nullptr, &pNpc)))
		return E_FAIL;

	pNavigation = pNpc->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr != pNavigation)
	{
		CNavigation::NAVIGATION_DESC NaviDesc;
		NaviDesc.bInitialize_Index = true;
		NaviDesc.vStartWorldPosition = _float4(-17.0f, -1.9f, -20.f, 1.f);

		if (FAILED(pNavigation->Initialize(&NaviDesc)))
			return E_FAIL;


		CTransform* pTransform = pNpc->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr != pTransform)
		{
			pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.f));
			pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pNavigation->Get_NaviDesc().vStartWorldPosition));
		}
	}

	// ´ëÈ­ ¿£ÇÇ¾¾

	if (FAILED(GI->Add_GameObject(LEVEL_TRAIN_STATION, LAYER_TYPE::LAYER_NPC, TEXT("Prototype_GameObject_Npc_Talk_PoliceMan_0"), nullptr, &pNpc)))
		return E_FAIL;

	pNavigation = pNpc->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr != pNavigation)
	{
		CNavigation::NAVIGATION_DESC NaviDesc;
		NaviDesc.bInitialize_Index = true;
		NaviDesc.vStartWorldPosition = _float4(44.5f, -0.5f, 37.7f, 1.f);

		if (FAILED(pNavigation->Initialize(&NaviDesc)))
			return E_FAIL;


		CTransform* pTransform = pNpc->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr != pTransform)
		{
			pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
			pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pNavigation->Get_NaviDesc().vStartWorldPosition));
		}
	}

	if (FAILED(GI->Add_GameObject(LEVEL_TRAIN_STATION, LAYER_TYPE::LAYER_NPC, TEXT("Prototype_GameObject_Npc_Cat_0"), nullptr, &pNpc)))
		return E_FAIL;

	pNavigation = pNpc->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr != pNavigation)
	{
		CNavigation::NAVIGATION_DESC NaviDesc;
		NaviDesc.bInitialize_Index = true;
		NaviDesc.vStartWorldPosition = _float4(-5.9f, -1.95f, -63.5f, 1.f);

		if (FAILED(pNavigation->Initialize(&NaviDesc)))
			return E_FAIL;


		CTransform* pTransform = pNpc->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr != pTransform)
		{
			pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.f));
			pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pNavigation->Get_NaviDesc().vStartWorldPosition));
		}
	}


	


	return S_OK;
}

HRESULT CLevel_Train_Station::Ready_Layer_BackGround(const LAYER_TYPE eLayerType)
{

	return S_OK;
}

HRESULT CLevel_Train_Station::Ready_Layer_Monster(const LAYER_TYPE eLayerType)
{
	/*if (FAILED(GI->Add_GameObject(LEVEL_TRAIN_STATION, LAYER_TYPE::LAYER_MONSTER, TEXT("Prototype_GameObject_NormalMonster_0"), nullptr)))
		return E_FAIL;*/

	//if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_TRAIN_STATION, LAYER_TYPE::LAYER_MONSTER, TEXT("Prototype_GameObject_NormalMonster_1"), nullptr)))
	//	return E_FAIL;

	//if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_TRAIN_STATION, LAYER_TYPE::LAYER_MONSTER, TEXT("Prototype_GameObject_NormalMonster_2"), nullptr)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Train_Station::Ready_Layer_UI(const LAYER_TYPE eLayerType)
{



	return S_OK;
}

HRESULT CLevel_Train_Station::Ready_Layer_Effect(const LAYER_TYPE eLayerType)
{

	return S_OK;
}

CLevel_Train_Station * CLevel_Train_Station::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Train_Station*	pInstance = new CLevel_Train_Station(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Train_Station");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Train_Station::Free()
{
	__super::Free();


}

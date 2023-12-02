#include "stdafx.h"
#include "..\Public\Level_Train_Boss.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Camera_Main.h"
#include "Character.h"

CLevel_Train_Boss::CLevel_Train_Boss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Train_Boss::Initialize()
{
	GI->Lock_Mouse();

	m_pRendererCom = dynamic_cast<CRenderer*>(GI->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Renderer"));
	if (nullptr == m_pRendererCom)
		return E_FAIL;
	
	m_vFogColor = { 0.f, 0.f, 0.f,1.f };
	m_vFogStartEnd = { 0.f, 120.f };
	m_pRendererCom->Set_FogColor(m_vFogColor);
	m_pRendererCom->Set_FogStartEnd(m_vFogStartEnd);

	if (FAILED(__super::Initialize()))
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

	m_ScrollObjectLayer.push_back(LAYER_TYPE::LAYER_TREE);
	m_ScrollObjectLayer.push_back(LAYER_TYPE::LAYER_PROP);
	m_ScrollObjectLayer.push_back(LAYER_TYPE::LAYER_GRASS);
	m_ScrollObjectLayer.push_back(LAYER_TYPE::LAYER_BUILDING);
	m_ScrollObjectLayer.push_back(LAYER_TYPE::LAYER_MOUNTAIN);
	m_ScrollObjectLayer.push_back(LAYER_TYPE::LAYER_GROUND);

	m_fScrollSpeed = 100.f;
	m_fLimitScroll = -2300.f;

	return S_OK;
}

HRESULT CLevel_Train_Boss::Tick(_float fTimeDelta)
{

	//if (KEY_TAP(KEY::U))
	//{
	//	m_vFogStartEnd.x -= 1.f;
	//	m_vFogStartEnd.x = max(0.f, m_vFogStartEnd.x);
	//	m_pRendererCom->Set_FogStartEnd(m_vFogStartEnd);
	//}

	//if (KEY_TAP(KEY::I))
	//{
	//	m_vFogStartEnd.x += 1.f;
	//	m_pRendererCom->Set_FogStartEnd(m_vFogStartEnd);
	//}

	//if (KEY_TAP(KEY::O))
	//{
	//	
	//	if (KEY_HOLD(KEY::SHIFT))
	//	{
	//		m_vFogStartEnd.y -= 1.f;
	//	}
	//	else
	//	{
	//		m_vFogStartEnd.y -= 10.f;
	//	}
	//	m_vFogStartEnd.y = max(0.f, m_vFogStartEnd.y);
	//	m_pRendererCom->Set_FogStartEnd(m_vFogStartEnd);
	//}

	//if (KEY_TAP(KEY::P))
	//{

	//	if (KEY_HOLD(KEY::SHIFT))
	//	{
	//		m_vFogStartEnd.y -= 1.f;
	//	}
	//	else
	//	{
	//		m_vFogStartEnd.y += 10.f;
	//	}
	//	
	//	m_pRendererCom->Set_FogStartEnd(m_vFogStartEnd);
	//}


	//if (KEY_TAP(KEY::R))
	//{
	//	if (KEY_HOLD(KEY::SHIFT))
	//	{
	//		m_vFogColor.x -= 0.1f;
	//		m_vFogColor.x = max(0.f, m_vFogColor.x);
	//	}
	//	else
	//	{
	//		m_vFogColor.x += 0.1f;
	//		m_vFogColor.x = min(m_vFogColor.x, 1.f);
	//	}
	//	m_pRendererCom->Set_FogColor(m_vFogColor);
	//}

	//if (KEY_TAP(KEY::G))
	//{
	//	if (KEY_HOLD(KEY::SHIFT))
	//	{
	//		m_vFogColor.y -= 0.1f;
	//		m_vFogColor.y = max(0.f, m_vFogColor.y);
	//	}
	//	else
	//	{
	//		m_vFogColor.y += 0.1f;
	//		m_vFogColor.y = min(m_vFogColor.y, 1.f);
	//	}
	//	m_pRendererCom->Set_FogColor(m_vFogColor);
	//}

	//if (KEY_TAP(KEY::B))
	//{
	//	if (KEY_HOLD(KEY::SHIFT))
	//	{
	//		m_vFogColor.z -= 0.1f;
	//		m_vFogColor.z = max(0.f, m_vFogColor.z);
	//	}
	//	else
	//	{
	//		m_vFogColor.z += 0.1f;
	//		m_vFogColor.z = min(m_vFogColor.z, 1.f);
	//	}
	//	m_pRendererCom->Set_FogColor(m_vFogColor);
	//}



	m_fAccScroll -= m_fScrollSpeed * fTimeDelta;
	if (m_fAccScroll <= m_fLimitScroll)
		Reset_Scroll();

	Scroll(fTimeDelta);

	return S_OK;
}

HRESULT CLevel_Train_Boss::LateTick(_float fTimeDelta)
{
	
	return S_OK;
}

HRESULT CLevel_Train_Boss::Enter_Level()
{
	return S_OK;
}

HRESULT CLevel_Train_Boss::Exit_Level()
{
	return S_OK;
}


HRESULT CLevel_Train_Boss::Ready_Lights()
{

	return S_OK;
}

HRESULT CLevel_Train_Boss::Ready_Layer_Camera(const LAYER_TYPE eLayerType)
{
	


	CCamera::CAMERADESC			CameraDesc;

	CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 1000.f;

	CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

 	if(FAILED(GI->Add_GameObject(LEVELID::LEVEL_TRAIN_BOSS, LAYER_CAMERA, TEXT("Prototype_GameObject_Camera_Main"), &CameraDesc)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObject(LEVELID::LEVEL_TRAIN_BOSS, LAYER_BACKGROUND, TEXT("Prototype_GameObject_Sky_Night"))))
		return E_FAIL;

	/*if (FAILED(GI->Add_GameObject(LEVELID::LEVEL_GAMEPLAY, LAYER_CAMERA, TEXT("Prototype_GameObject_Camera_Main"), &CameraDesc)))
		return E_FAIL;*/

	

	return S_OK;
}

HRESULT CLevel_Train_Boss::Ready_Layer_Player(const LAYER_TYPE eLayerType)
{
	/*CGameObject* pTanjiro = nullptr;
	if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_TRAIN_BOSS, LAYER_TYPE::LAYER_CHARACTER, TEXT("Prototype_GameObject_Tanjiro"), nullptr, &pTanjiro)))
		return E_FAIL;

	CGameObject* pObject = GI->Find_GameObject(LEVELID::LEVEL_TRAIN_BOSS, LAYER_CAMERA, L"Main_Camera");
	if (nullptr == pObject)
		return E_FAIL;

	CCamera_Main* pCamera = dynamic_cast<CCamera_Main*>(pObject);
	if (nullptr == pCamera)
		return E_FAIL;

	CCharacter* pCharacter = dynamic_cast<CCharacter*>(pTanjiro);
	if (nullptr == pCharacter)
		return E_FAIL;*/




	CGameObject* pZenitsu = nullptr;
	if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_TRAIN_BOSS, LAYER_TYPE::LAYER_CHARACTER, TEXT("Prototype_GameObject_Zenitsu"), nullptr, &pZenitsu)))
		return E_FAIL;

	CGameObject* pObject = GI->Find_GameObject(LEVELID::LEVEL_TRAIN_BOSS, LAYER_CAMERA, L"Main_Camera");
	if (nullptr == pObject)
		return E_FAIL;

	CCamera_Main* pCamera = dynamic_cast<CCamera_Main*>(pObject);
	if (nullptr == pCamera)
		return E_FAIL;

	CCharacter* pCharacter = dynamic_cast<CCharacter*>(pZenitsu);
	if (nullptr == pCharacter)
		return E_FAIL;



	if (FAILED(pCamera->Set_TargetTransform(pCharacter->Get_Component<CTransform>(L"Com_Transform"))))
		return E_FAIL;

	CNavigation* pNavigation = pCharacter->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr != pNavigation)
	{
		CNavigation::NAVIGATION_DESC NavigationDesc;
		NavigationDesc.bInitialize_Index = true;
		NavigationDesc.vStartWorldPosition = _float4(0.f, 15.f, 40.f, 1.f);
		if (FAILED(pNavigation->Initialize(&NavigationDesc)))
			return E_FAIL;

		pCharacter->Get_Component<CTransform>(L"Com_Transform")->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pNavigation->Get_NaviDesc().vStartWorldPosition));
	}

	return S_OK;
}

HRESULT CLevel_Train_Boss::Ready_Layer_Character(const LAYER_TYPE eLayerType)
{
	return S_OK;
}

HRESULT CLevel_Train_Boss::Ready_Layer_BackGround(const LAYER_TYPE eLayerType)
{

	return S_OK;
}

HRESULT CLevel_Train_Boss::Ready_Layer_Monster(const LAYER_TYPE eLayerType)
{
	// CGameObject* pBoss = GI->Clone_GameObject(TEXT("Prototype_GameObject_Enmu"), LAYER_MONSTER);
	CGameObject* pBoss = GI->Clone_GameObject(TEXT("Prototype_GameObject_Akaza"), LAYER_MONSTER);


	CTransform* pTransform = pBoss->Get_Component<CTransform>(L"Com_Transform");
	CNavigation* pNavigation = pBoss->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr == pNavigation || nullptr == pTransform)
		return E_FAIL;

	pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 10.6f, 185.f, 1.f));

	CNavigation::NAVIGATION_DESC NaviDesc;
	NaviDesc.bInitialize_Index = true;
	XMStoreFloat4(&NaviDesc.vStartWorldPosition, pTransform->Get_State(CTransform::STATE_POSITION));

	if (FAILED(pNavigation->Initialize(&NaviDesc)))
		MSG_BOX("Gen TrainBoss FAILED.");
	else
		pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&NaviDesc.vStartWorldPosition));

	
	if (FAILED(GI->Add_GameObject(LEVEL_TRAIN_BOSS, LAYER_TYPE::LAYER_MONSTER, pBoss)))
		return E_FAIL;	

	

	return S_OK;
}

HRESULT CLevel_Train_Boss::Ready_Layer_UI(const LAYER_TYPE eLayerType)
{



	return S_OK;
}

HRESULT CLevel_Train_Boss::Ready_Layer_Effect(const LAYER_TYPE eLayerType)
{

	return S_OK;
}

void CLevel_Train_Boss::Reset_Scroll()
{
	for (auto& Layer : m_ScrollObjectLayer)
	{
		const list<class CGameObject*>& GameObjects = GI->Find_GameObjects(GI->Get_CurrentLevel(), Layer);

		for (auto& Object : GameObjects)
		{
			if (Object->Get_ObjectTag().find(L"Locomotive") != wstring::npos)
				continue;

			CTransform* pTransform = Object->Get_Component<CTransform>(L"Com_Transform");
			if (nullptr == pTransform)
				continue;

			_vector vPosition = pTransform->Get_State(CTransform::STATE::STATE_POSITION);
			vPosition = XMVectorSetZ(vPosition, XMVectorGetZ(vPosition) + fabs(m_fAccScroll));
			pTransform->Set_State(CTransform::STATE::STATE_POSITION, vPosition);
		}
	}

	m_fAccScroll = 0.f;
}

void CLevel_Train_Boss::Scroll(_float fTimeDelta)
{
	for (auto& Layer : m_ScrollObjectLayer)
	{
		const list<class CGameObject*>& GameObjects = GI->Find_GameObjects(GI->Get_CurrentLevel(), Layer);

		for (auto& Object : GameObjects)
		{
			if (Object->Get_ObjectTag().find(L"Locomotive") != wstring::npos)
				continue;

			CTransform* pTransform = Object->Get_Component<CTransform>(L"Com_Transform");
			if (nullptr == pTransform)
				continue;


			_vector vPosition = pTransform->Get_State(CTransform::STATE::STATE_POSITION);
			vPosition = XMVectorSetZ(vPosition, XMVectorGetZ(vPosition) - (m_fScrollSpeed * fTimeDelta));
			pTransform->Set_State(CTransform::STATE::STATE_POSITION, vPosition);
		}
	}
}

CLevel_Train_Boss * CLevel_Train_Boss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Train_Boss*	pInstance = new CLevel_Train_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Train_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Train_Boss::Free()
{
	__super::Free();


}

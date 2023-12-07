#include "stdafx.h"
#include "..\Public\Level_Train.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "Character.h"
#include "GameObject.h"



CLevel_Train::CLevel_Train(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Train::Initialize()
{
	GI->Stop_All();
	GI->Lock_Mouse();

	m_pRendererCom = dynamic_cast<CRenderer*>(GI->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Renderer"));
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_vFogColor = { .4f, .3f, .2f, 1.f };
	m_pRendererCom->Set_FogColor(m_vFogColor);
	m_pRendererCom->Set_FogStartEnd(m_vFogStartEnd);

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_TYPE::LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(LAYER_TYPE::LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Npc(LAYER_TYPE::LAYER_NPC)))
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

	m_fScrollSpeed = 50.f;
	m_fLimitScroll = -1500.f;
	return S_OK;
}

HRESULT CLevel_Train::Tick(_float fTimeDelta)
{
	m_fAccScroll -= m_fScrollSpeed * fTimeDelta;
	if (m_fAccScroll <= m_fLimitScroll)
		Reset_Scroll();

	Scroll(fTimeDelta);



	/*if (KEY_TAP(KEY::R))
	{
		if (KEY_HOLD(KEY::SHIFT))
		{
			m_vFogColor.x -= 0.1f;
			m_vFogColor.x = max(0.f, m_vFogColor.x);
		}
		else
		{
			m_vFogColor.x += 0.1f;
			m_vFogColor.x = min(m_vFogColor.x, 1.f);
		}
		m_pRendererCom->Set_FogColor(m_vFogColor);
	}

	if (KEY_TAP(KEY::G))
	{
		if (KEY_HOLD(KEY::SHIFT))
		{
			m_vFogColor.y -= 0.1f;
			m_vFogColor.y = max(0.f, m_vFogColor.y);
		}
		else
		{
			m_vFogColor.y += 0.1f;
			m_vFogColor.y = min(m_vFogColor.y, 1.f);
		}
		m_pRendererCom->Set_FogColor(m_vFogColor);
	}

	if (KEY_TAP(KEY::B))
	{
		if (KEY_HOLD(KEY::SHIFT))
		{
			m_vFogColor.z -= 0.1f;
			m_vFogColor.z = max(0.f, m_vFogColor.z);
		}
		else
		{
			m_vFogColor.z += 0.1f;
			m_vFogColor.z = min(m_vFogColor.z, 1.f);
		}
		m_pRendererCom->Set_FogColor(m_vFogColor);
	}*/

	
	
	return S_OK;
}

HRESULT CLevel_Train::LateTick(_float fTimeDelta)
{
	
	return S_OK;
}

HRESULT CLevel_Train::Enter_Level()
{
	return S_OK;
}

HRESULT CLevel_Train::Exit_Level()
{
	return S_OK;
}

HRESULT CLevel_Train::Ready_Lights()
{


	return S_OK;
}

HRESULT CLevel_Train::Ready_Layer_Camera(const LAYER_TYPE eLayerType)
{
	CCamera_Manager::GetInstance()->Set_MainCamera(CCamera_Manager::CAMERA_TYPE::GAME_PLAY);
	
	if (FAILED(GI->Add_GameObject(LEVELID::LEVEL_TRAIN, LAYER_BACKGROUND, TEXT("Prototype_GameObject_Sky_Night"))))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CLevel_Train::Ready_Layer_Player(const LAYER_TYPE eLayerType)
{
	CGameObject* pTanjiro = nullptr;
	if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_TRAIN, LAYER_TYPE::LAYER_CHARACTER, TEXT("Prototype_GameObject_Tanjiro"), nullptr, &pTanjiro)))
		return E_FAIL;

	CCharacter* pCharacter = dynamic_cast<CCharacter*>(pTanjiro);
	if (nullptr == pCharacter)
		return E_FAIL;


	//CGameObject* pZenitsu = nullptr;
	//if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_GAMEPLAY, LAYER_TYPE::LAYER_CHARACTER, TEXT("Prototype_GameObject_Zenitsu"), nullptr, &pZenitsu)))
	//	return E_FAIL;

	//CCharacter* pCharacter = dynamic_cast<CCharacter*>(pZenitsu);
	//if (nullptr == pCharacter)
	//	return E_FAIL;

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
	
	CCamera* pMainCamera = CCamera_Manager::GetInstance()->Get_MainCamera();
	if (FAILED(pMainCamera->Set_TargetTransform(pCharacter->Get_Component<CTransform>(L"Com_Transform"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Train::Ready_Layer_Character(const LAYER_TYPE eLayerType)
{
	return S_OK;
}

HRESULT CLevel_Train::Ready_Layer_BackGround(const LAYER_TYPE eLayerType)
{
	return S_OK;
}

HRESULT CLevel_Train::Ready_Layer_Npc(const LAYER_TYPE eLayerType)
{
 	if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_TRAIN, LAYER_TYPE::LAYER_NPC, TEXT("Prototype_GameObject_Npc_Defence_Zenitsu"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Train::Ready_Layer_Monster(const LAYER_TYPE eLayerType)
{
	return S_OK;
}

HRESULT CLevel_Train::Ready_Layer_UI(const LAYER_TYPE eLayerType)
{



	return S_OK;
}

HRESULT CLevel_Train::Ready_Layer_Effect(const LAYER_TYPE eLayerType)
{

	return S_OK;
}


void CLevel_Train::Reset_Scroll()
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

void CLevel_Train::Scroll(_float fTimeDelta)
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

CLevel_Train * CLevel_Train::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Train*	pInstance = new CLevel_Train(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Train");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Train::Free()
{
	__super::Free();


}

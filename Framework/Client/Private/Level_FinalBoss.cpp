#include "stdafx.h"
#include "..\Public\Level_FinalBoss.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "Character.h"
#include "Monster.h"
#include "UI_Manager.h"

CLevel_FinalBoss::CLevel_FinalBoss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_FinalBoss::Initialize()
{
	GI->Lock_Mouse();

	m_pRendererCom = dynamic_cast<CRenderer*>(GI->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Renderer"));
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_vFogColor = { 0.1f, 0.1f, 0.1f, 1.f };
	m_vFogStartEnd = { 0.f, 40.f };
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

	if (FAILED(Ready_Layer_Character(LAYER_TYPE::LAYER_CHARACTER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(LAYER_TYPE::LAYER_MONSTER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(LAYER_TYPE::LAYER_EFFECT)))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_TYPE::LAYER_UI)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_FinalBoss::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_FinalBoss::LateTick(_float fTimeDelta)
{
	
	return S_OK;
}

HRESULT CLevel_FinalBoss::Enter_Level()
{
	GI->Stop_All();
	GI->Play_BGM(L"Final_Boss.wav", 1.f, true);
	return S_OK;
	return S_OK;
}

HRESULT CLevel_FinalBoss::Exit_Level()
{
	return S_OK;
}


HRESULT CLevel_FinalBoss::Ready_Lights()
{

	return S_OK;
}

HRESULT CLevel_FinalBoss::Ready_Layer_Camera(const LAYER_TYPE eLayerType)
{
	CCamera_Manager::GetInstance()->Set_MainCamera(CCamera_Manager::CAMERA_TYPE::GAME_PLAY);
	if (FAILED(GI->Add_GameObject(LEVELID::LEVEL_FINAL_BOSS, LAYER_BACKGROUND, TEXT("Prototype_GameObject_Sky_Night"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_FinalBoss::Ready_Layer_Player(const LAYER_TYPE eLayerType)
{


	return S_OK;
}

HRESULT CLevel_FinalBoss::Ready_Layer_Character(const LAYER_TYPE eLayerType)
{
	CGameObject* pKyojuro = nullptr;
	if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_FINAL_BOSS, LAYER_TYPE::LAYER_CHARACTER, TEXT("Prototype_GameObject_Kyojuro"), nullptr, &pKyojuro)))
		return E_FAIL;

	CCharacter* pCharacter = dynamic_cast<CCharacter*>(pKyojuro);
	if (nullptr == pCharacter)
		return E_FAIL;


	CNavigation* pNavigation = pCharacter->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr != pNavigation)
	{
		CNavigation::NAVIGATION_DESC NavigationDesc;
		NavigationDesc.bInitialize_Index = true;
		NavigationDesc.vStartWorldPosition = _float4(-5.f, 1.f, 88.f, 1.f);
		if (FAILED(pNavigation->Initialize(&NavigationDesc)))
			return E_FAIL;

		pCharacter->Get_Component<CTransform>(L"Com_Transform")->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pNavigation->Get_NaviDesc().vStartWorldPosition));
	}

	CCamera* pMainCamera = CCamera_Manager::GetInstance()->Get_MainCamera();
	if (FAILED(pMainCamera->Set_TargetTransform(pCharacter->Get_Component<CTransform>(L"Com_Transform"))))
		return E_FAIL;

	if (FAILED(CUI_Manager::GetInstance()->Reserve_HpBar(CUI_Manager::GAUGE_BARTYPE::LEFT_HP, pCharacter, CCharacter::CHARACTER_TYPE::KYOJURO)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_FinalBoss::Ready_Layer_BackGround(const LAYER_TYPE eLayerType)
{

	return S_OK;
}

HRESULT CLevel_FinalBoss::Ready_Layer_Monster(const LAYER_TYPE eLayerType)
{
	CGameObject* pBoss = GI->Clone_GameObject(TEXT("Prototype_GameObject_Akaza"), LAYER_MONSTER);


	CNavigation* pNavigation = pBoss->Get_Component<CNavigation>(L"Com_Navigation");
	if (nullptr != pNavigation)
	{
		CNavigation::NAVIGATION_DESC NavigationDesc;
		NavigationDesc.bInitialize_Index = true;
		NavigationDesc.vStartWorldPosition = _float4(42.f, 1.f, 94.f, 1.f);
		if (FAILED(pNavigation->Initialize(&NavigationDesc)))
			return E_FAIL;

		pBoss->Get_Component<CTransform>(L"Com_Transform")->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pNavigation->Get_NaviDesc().vStartWorldPosition));
	}


	if (FAILED(GI->Add_GameObject(LEVEL_FINAL_BOSS, LAYER_TYPE::LAYER_MONSTER, pBoss)))
		return E_FAIL;

	CMonster* pMonster = dynamic_cast<CMonster*>(pBoss);
	if (nullptr == pMonster)
		return E_FAIL;

	CUI_Manager::GetInstance()->Reserve_HpBar(CUI_Manager::GAUGE_BARTYPE::RIGHT_HP, pMonster, CCharacter::CHARACTER_TYPE::AKAZA);
	
	

	return S_OK;
}

HRESULT CLevel_FinalBoss::Ready_Layer_UI(const LAYER_TYPE eLayerType)
{



	return S_OK;
}

HRESULT CLevel_FinalBoss::Ready_Layer_Effect(const LAYER_TYPE eLayerType)
{

	return S_OK;
}

CLevel_FinalBoss * CLevel_FinalBoss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_FinalBoss*	pInstance = new CLevel_FinalBoss(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_FinalBoss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_FinalBoss::Free()
{
	__super::Free();


}

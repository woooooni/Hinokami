#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "VIBuffer_Trail.h"
#include "Level_Loading.h"
#include "Network_Manager.h"
#include "SocketUtils.h"
#include "ImGui_Manager.h"
#include "Picking_Manager.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"

#include "UI_Loading_Anim.h"
#include "UI_Loading_Background.h"
#include "UI_Loading_Icon.h"


CMainApp::CMainApp()	
	: m_pGame_Instance(CGameInstance::GetInstance())
	, m_pNetwork_Manager(CNetwork_Manager::GetInstance())
{
	Safe_AddRef(m_pGame_Instance);
}


HRESULT CMainApp::Initialize()
{
	/* 1. 내 게임의 초기화를 수행할꺼야. */
	/* 1-1. 그래픽장치를 초기화한다. */
	/* 1-2. 사운드장치를 초기화한다. */
	/* 1-3. 입력장치를 초기화한다. */
	GRAPHIC_DESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.eWinMode = GRAPHIC_DESC::WINMODE_WIN;
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;

	SocketUtils::Init();

	if (FAILED(m_pGame_Instance->Initialize_Engine(LEVEL_END, _uint(LAYER_TYPE::LAYER_END), GraphicDesc, &m_pDevice, &m_pContext, g_hWnd, g_hInstance)))
		return E_FAIL;

	if (FAILED(Initialize_Client()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	/* 1-4. 게임내에서 사용할 레벨(씬)을 생성한다.   */
	if (FAILED(Open_Level(LEVEL_GAMEPLAY)))
		return E_FAIL;


	/* 1-4-1. 게임내에서 사용할 여러 자원(텍스쳐, 모델, 객체) 들을 준비한다.  */

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pGame_Instance->Tick(fTimeDelta);
	m_fTimeAcc += fTimeDelta;
	
}

HRESULT CMainApp::Render()
{
	/* 게임내에 존재하는 여러 객체들의 렌더링. */
	m_pGame_Instance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGame_Instance->Clear_DepthStencil_View();

	m_pRenderer_Com->Draw();
	m_pGame_Instance->Render_Debug();
	/* 초기화한 장면에 객체들을 그린다. */
	m_pGame_Instance->Present();

	++m_iNumDraw;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("fps : %d"), m_iNumDraw);
		m_iNumDraw = 0;
		m_fTimeAcc = 0.f;
	}

	SetWindowText(g_hWnd, m_szFPS);

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelID)
{
	if (nullptr == m_pGame_Instance)
		return E_FAIL;

	/* 로고레베릉ㄹ 할당하고 싶었지만. 로고레벨을 위한 로딩레벨을 먼저 생성하여 로딩작업을 수행할꺼야. */
	/* 로딩객체에게 eLevelID라는 내가 실제 할당ㅎ아고 싶었던 레벨열거체를 준거지?! 실제할당하고싶었던 레벨에 자원을 준비라하라고 */
	if (FAILED(m_pGame_Instance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Initialize_Client()
{
	if(FAILED(GI->Add_Fonts(m_pDevice, m_pContext, L"Batang", L"../Bin/Resources/Font/Batang.spritefont")))
		return E_FAIL;
	if(FAILED(GI->Add_Fonts(m_pDevice, m_pContext, L"Maple", L"../Bin/Resources/Font/Maplestory.spritefont")))
		return E_FAIL;


	CUI::UI_INFO tInfo = {};

	tInfo.fX = g_iWinSizeX / 2.f;
	tInfo.fY = g_iWinSizeY / 2.f;
	tInfo.fCX = g_iWinSizeX;
	tInfo.fCY = g_iWinSizeY;

	///* For.Prototype_GameObject_UI_Loading_BackGround */
	if (FAILED(GAME_INSTANCE->Add_Prototype(TEXT("Prototype_GameObject_UI_Loading_BackGround"), CUI_Loading_BackGround::Create(m_pDevice, m_pContext, tInfo), LAYER_TYPE::LAYER_UI)))
		return E_FAIL;

	tInfo.fX = g_iWinSizeX - 300.f;
	tInfo.fY = g_iWinSizeY - 150.f;
	tInfo.fCX = 244.f;
	tInfo.fCY = 256.f;

	///* For.Prototype_GameObject_UI_Loading_Anim */
	if (FAILED(GAME_INSTANCE->Add_Prototype(TEXT("Prototype_GameObject_UI_Loading_Anim"), CUI_Loading_Anim::Create(m_pDevice, m_pContext, tInfo), LAYER_TYPE::LAYER_UI)))
		return E_FAIL;

	tInfo.fX = g_iWinSizeX - 400.f;
	tInfo.fY = g_iWinSizeY - 50.f;
	tInfo.fCX = 120.f;
	tInfo.fCY = 120.f;

	///* For.Prototype_GameObject_UI_Loading_Icon */
	if (FAILED(GAME_INSTANCE->Add_Prototype(TEXT("Prototype_GameObject_UI_Loading_Icon"), CUI_Loading_Icon::Create(m_pDevice, m_pContext, tInfo), LAYER_TYPE::LAYER_UI)))
		return E_FAIL;



	if (FAILED(CEffect_Manager::GetInstance()->Reserve_Manager(m_pDevice, m_pContext, L"../Bin/Export/Effect/")))
		return E_FAIL;

	if(FAILED(CParticle_Manager::GetInstance()->Reserve_Manager(m_pDevice, m_pContext, L"../Bin/Export/Particle/")))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	if (nullptr == m_pGame_Instance)
		return E_FAIL;

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), m_pRenderer_Com = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer_Com);


	///* For.Prototype_Component_VIBuffer_Particle */
	//if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle"),
	//	CVIBuffer_Particle::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, L"../Bin/DataFiles/Map/Temp/Temp.nav"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 256, 256))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Trail*/
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext, 220))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_UI*/
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UI.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Mesh_Effect */
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Mesh_Effect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshEffect.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Texture_Effect */
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Texture_Effect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTextureEffect.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Trail */
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Model*/
	if (FAILED(GAME_INSTANCE->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_AnimModel */
	if (FAILED(GAME_INSTANCE->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;


	// Texture
	/* For.Prototype_Component_Texture_Effect*/
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Texture/"), 0, true))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Trail*/
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Trail/"), 0, true))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Dissolve*/
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Dissolve/"), 0, true))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NextFog"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/NextFog/"), 0, true))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Loading/Loading_Background/"), 0, true))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Anim"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Loading/Loading_Anim/"), 0, true))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Loading/Loading_Icon/"), 0, true))))
		return E_FAIL;




	/* For.Prototype_Component_Transform */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_StateMachine */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"),
		CStateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_RigidBody */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		CRigidBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_AABB_Collider */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sphere_Collider"),
		CCollider_Sphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_AABB_Collider"),
		CCollider_AABB::Create(m_pDevice, m_pContext))))
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

	Safe_Release(m_pRenderer_Com);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	CPicking_Manager::GetInstance()->DestroyInstance();
	CImGui_Manager::GetInstance()->DestroyInstance();
	CNetwork_Manager::GetInstance()->DestroyInstance();

	Safe_Release(m_pGame_Instance);
	CGameInstance::Release_Engine();

	SocketUtils::Clear();
}

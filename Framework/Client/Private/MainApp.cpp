#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "ImGui_Manager.h"

CMainApp::CMainApp()	
	: m_pGame_Instance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGame_Instance);
}


HRESULT CMainApp::Initialize()
{
	/* 1. �� ������ �ʱ�ȭ�� �����Ҳ���. */
	/* 1-1. �׷�����ġ�� �ʱ�ȭ�Ѵ�. */
	/* 1-2. ������ġ�� �ʱ�ȭ�Ѵ�. */
	/* 1-3. �Է���ġ�� �ʱ�ȭ�Ѵ�. */
	GRAPHIC_DESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.eWinMode = GRAPHIC_DESC::WINMODE_WIN;
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;

	if (FAILED(m_pGame_Instance->Initialize_Engine(LEVEL_END, _uint(LAYER_TYPE::LAYER_END), GraphicDesc, &m_pDevice, &m_pContext, g_hWnd, g_hInstance)))
		return E_FAIL;

	if (FAILED(Initialize_Client()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	/* 1-4. ���ӳ����� ����� ����(��)�� �����Ѵ�.   */
	if (FAILED(Open_Level(LEVEL_TOOL)))
		return E_FAIL;


	/* 1-4-1. ���ӳ����� ����� ���� �ڿ�(�ؽ���, ��, ��ü) ���� �غ��Ѵ�.  */

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	

	/* ���ӳ��� �����ϴ� ���� ��ü���� ����. */
	/* ������ ���� */
	m_pGame_Instance->Tick(fTimeDelta);
	
}

HRESULT CMainApp::Render()
{
	/* ���ӳ��� �����ϴ� ���� ��ü���� ������. */
	m_pGame_Instance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGame_Instance->Clear_DepthStencil_View();

	m_pRenderer_Com->Draw();
	m_pGame_Instance->Render_Debug();
	/* �ʱ�ȭ�� ��鿡 ��ü���� �׸���. */
	m_pGame_Instance->Present();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelID)
{
	if (nullptr == m_pGame_Instance)
		return E_FAIL;

	/* �ΰ������� �Ҵ��ϰ� �;�����. �ΰ����� ���� �ε������� ���� �����Ͽ� �ε��۾��� �����Ҳ���. */
	/* �ε���ü���� eLevelID��� ���� ���� �Ҵ社�ư� �;��� ��������ü�� �ذ���?! �����Ҵ��ϰ�;��� ������ �ڿ��� �غ���϶�� */
	if (FAILED(m_pGame_Instance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Initialize_Client()
{
	// Manager Reserve
	if(FAILED(CImGui_Manager::GetInstance()->Reserve_Manager(g_hWnd, m_pDevice, m_pContext)))
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

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 128, 128))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Mesh_Effect */
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Mesh_Effect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshEffect.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Model*/
	if (FAILED(GAME_INSTANCE->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_AnimModel */
	if (FAILED(GAME_INSTANCE->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain*/
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile0.jpg")))))
		return E_FAIL;

	/* For.Prototype_Component_Effect_Texture*/
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Effect_Texture"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effects/T_e_Plc_P0012_Fire020.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_StateMachine */
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"),
		CStateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGame_Instance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
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


	CImGui_Manager::GetInstance()->DestroyInstance();


	Safe_Release(m_pGame_Instance);
	CGameInstance::Release_Engine();
}

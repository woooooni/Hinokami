#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Light_Manager.h"

#include "GameInstance.h"


CRenderer::CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);

}

HRESULT CRenderer::Initialize_Prototype()
{
	D3D11_VIEWPORT		ViewportDesc;

	_uint				iNumViewports = 1;

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* For.Target_Diffuse */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;


	/* For.Target_Depth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Specular */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Blur_DownSampling */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Blur"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	/* For.Target_Blur_DownSampling */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Blur_DownSampling"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Blur_XY */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Blur_XY"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Blur_UpSampling */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Blur_UpSampling"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	/* For.Target_ShadowDepth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_ShadowDepth"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	//Diffuse & Normal
	{
		/* For.MRT_GameObjects */
		if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
			return E_FAIL;
	}

	// Shade & Specular
	{
		/* For.MRT_Lights */
		if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Shade"))))
			return E_FAIL;

		if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Specular"))))
			return E_FAIL;
	}
	


	//Blur
	{
		/* For.MRT_BlurHorizontal */
		if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Blur"), TEXT("Target_Blur"))))

			return E_FAIL;
		/* For.MRT_BlurDownSampling */
		if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurDownSampling"), TEXT("Target_Blur_DownSampling"))))
			return E_FAIL;

		/* For.MRT_BlurHorizontal */
		if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurXY"), TEXT("Target_Blur_XY"))))
			return E_FAIL;

		/* For.MRT_BlurUpSampling */
		if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurUpSampling"), TEXT("Target_Blur_UpSampling"))))
			return E_FAIL;
	}
	

	// Shadow
	{
		/* For.MRT_Shadow */
		if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_ShadowDepth"))))
			return E_FAIL;
	}
	



	// Debug_Ready

	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Diffuse"), 100.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Normal"), 100.f, 300.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Depth"), 100.f, 500.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Shade"), 300.f, 100.f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Specular"), 300.f, 300.f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_ShadowDepth"), 300.f, 500.f, 200.f, 200.f)))
		return E_FAIL;



	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Blur"), 100.f, 700.f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Blur_XY"), 300.f, 700.f, 200.f, 200.f)))
		return E_FAIL;





	m_pVIBufferInstance = CVIBuffer_Instancing::Create(m_pDevice, m_pContext);
	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	if (nullptr == m_pVIBufferInstance)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX_DECLARATION::Elements, VTXPOSTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));
	
	return S_OK;
}

HRESULT CRenderer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	auto iter = m_RenderObjects[eRenderGroup].find(pGameObject->Get_PrototypeTag());
	if (iter == m_RenderObjects[eRenderGroup].end())
	{
		vector<CGameObject*> Temp;
		Temp.reserve(1000);
		m_RenderObjects[eRenderGroup].emplace(pGameObject->Get_PrototypeTag(), Temp);

		iter = m_RenderObjects[eRenderGroup].find(pGameObject->Get_PrototypeTag());
	}

	iter->second.push_back(pGameObject);
	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw()
{
	if(FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_Shadow()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if(FAILED(Render_NonAlphaBlend()))
		return E_FAIL;

	if(FAILED(Render_Lights()))
		return E_FAIL;

	if(FAILED(Render_Deferred()))
		return E_FAIL;

	if(FAILED(Render_AlphaBlend()))
		return E_FAIL;

	if (FAILED(Render_Effect()))
		return E_FAIL;

	if(FAILED(Render_UI()))
		return E_FAIL;

	if (FAILED(Render_Debug()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto GameObjects : m_RenderObjects[RENDER_PRIORITY])
	{
		vector<_float4x4> WorldMatrices;
		WorldMatrices.reserve(GameObjects.second.size());

		CGameObject* pFirstGameObject = GameObjects.second[0];
		Safe_AddRef(pFirstGameObject);
		for (auto& pGameObject : GameObjects.second)
		{
			CTransform* pTransform = pGameObject->Get_Component<CTransform>(L"Com_Transform");
			WorldMatrices.push_back(pTransform->Get_WorldFloat4x4());

			Safe_Release(pGameObject);
		}

		if (FAILED(pFirstGameObject->Render(m_pVIBufferInstance, WorldMatrices)))
			return E_FAIL;

		Safe_Release(pFirstGameObject);
		GameObjects.second.clear();
	}

	return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
	/*if (FAILED(m_pTarget_Manager->Begin_Shadow_MRT(m_pContext, TEXT("MRT_Shadow"))))
		return E_FAIL;*/

	for (auto GameObjects : m_RenderObjects[RENDER_SHADOW])
	{
		vector<_float4x4> WorldMatrices;
		WorldMatrices.reserve(GameObjects.second.size());

		CGameObject* pFirstGameObject = GameObjects.second[0];
		Safe_AddRef(pFirstGameObject);
		for (auto& pGameObject : GameObjects.second)
		{
			CTransform* pTransform = pGameObject->Get_Component<CTransform>(L"Com_Transform");
			WorldMatrices.push_back(pTransform->Get_WorldFloat4x4());

			Safe_Release(pGameObject);
		}

		if (FAILED(pFirstGameObject->Render_ShadowDepth(m_pVIBufferInstance, WorldMatrices)))
			return E_FAIL;

		Safe_Release(pFirstGameObject);
		GameObjects.second.clear();
	}

	return S_OK;
}


HRESULT CRenderer::Render_NonLight()
{
	for (auto GameObjects : m_RenderObjects[RENDER_NONLIGHT])
	{
		vector<_float4x4> WorldMatrices;
		WorldMatrices.reserve(GameObjects.second.size());

		CGameObject* pFirstGameObject = GameObjects.second[0];
		Safe_AddRef(pFirstGameObject);
		for (auto& pGameObject : GameObjects.second)
		{
			CTransform* pTransform = pGameObject->Get_Component<CTransform>(L"Com_Transform");
			WorldMatrices.push_back(pTransform->Get_WorldFloat4x4());

			Safe_Release(pGameObject);
		}

		if (FAILED(pFirstGameObject->Render(m_pVIBufferInstance, WorldMatrices)))
			return E_FAIL;

		Safe_Release(pFirstGameObject);
		GameObjects.second.clear();
	}

	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto GameObjects : m_RenderObjects[RENDER_NONBLEND])
	{
		vector<_float4x4> WorldMatrices;
		WorldMatrices.reserve(GameObjects.second.size());

		CGameObject* pFirstGameObject = GameObjects.second[0];
		Safe_AddRef(pFirstGameObject);
		for (auto& pGameObject : GameObjects.second)
		{
			CTransform* pTransform = pGameObject->Get_Component<CTransform>(L"Com_Transform");
			WorldMatrices.push_back(pTransform->Get_WorldFloat4x4());

			Safe_Release(pGameObject);
		}

		if (FAILED(pFirstGameObject->Render(m_pVIBufferInstance, WorldMatrices)))
			return E_FAIL;

		Safe_Release(pFirstGameObject);
		GameObjects.second.clear();
	}
	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Lights"))))
		return E_FAIL;

	/* 사각형 버퍼를 직교투영으로 Shade타겟의 사이즈만큼 꽉 채워서 그릴꺼야. */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	
	_float4x4 ProjMatrixInv, ViewMatrixInv;
	ProjMatrixInv = GI->Get_TransformMatrixInverse_Float4x4(CPipeLine::D3DTS_PROJ);
	ViewMatrixInv = GI->Get_TransformMatrixInverse_Float4x4(CPipeLine::D3DTS_VIEW);

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &ViewMatrixInv)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &ProjMatrixInv)))
		return E_FAIL;
	
	_float4 vCamPosition = GAME_INSTANCE->Get_CamPosition();
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Normal"), "g_NormalTarget")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Depth"), "g_DepthTarget")))
		return E_FAIL;

	m_pLight_Manager->Render(m_pShader, m_pVIBuffer);

	/* 다시 장치의 0번째 소켓에 백 버퍼를 올린다. */
	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}


HRESULT CRenderer::Render_Deferred()
{
	/* 디퓨즈 타겟과 셰이드 타겟을 서로 곱하여 백버퍼에 최종적으로 찍어낸다. */
	_float4 vCamPosition = GI->Get_CamPosition();
	
	

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float4x4 ViewMatrixInv;
	ViewMatrixInv = GI->Get_TransformMatrixInverse_Float4x4(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &ViewMatrixInv)))
		return E_FAIL;

	_float4x4 ProjMatrixInv;
	ProjMatrixInv = GI->Get_TransformMatrixInverse_Float4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &ProjMatrixInv)))
		return E_FAIL;



	_float4x4 CamProjMatrix = GI->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShader->Bind_Matrix("g_CamProjMatrix", &CamProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTarget")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Shade"), "g_ShadeTarget")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Specular"), "g_SpecularTarget")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Depth"), "g_DepthTarget")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_ShadowDepth"), "g_ShadowTarget")))
		return E_FAIL;


	// TODO :: 그림자.
	_float4x4		LightMatix = GI->Get_ShadowViewMatrix(GI->Get_CurrentLevel());

	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &LightMatix)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	

	return S_OK;
}



HRESULT CRenderer::Render_AlphaBlend()
{


	for (auto GameObjects : m_RenderObjects[RENDER_ALPHABLEND])
	{
		vector<_float4x4> WorldMatrices;
		WorldMatrices.reserve(GameObjects.second.size());

		CGameObject* pFirstGameObject = GameObjects.second[0];
		Safe_AddRef(pFirstGameObject);
		for (auto& pGameObject : GameObjects.second)
		{
			CTransform* pTransform = pGameObject->Get_Component<CTransform>(L"Com_Transform");
			WorldMatrices.push_back(pTransform->Get_WorldFloat4x4());

			Safe_Release(pGameObject);
		}

		if (FAILED(pFirstGameObject->Render(m_pVIBufferInstance, WorldMatrices)))
			return E_FAIL;

		Safe_Release(pFirstGameObject);
		GameObjects.second.clear();
	}

	
	return S_OK;
}

HRESULT CRenderer::Render_Effect()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Blur"))))
		return E_FAIL;

	
	for (auto GameObjects : m_RenderObjects[RENDER_EFFECT])
	{
		vector<_float4x4> WorldMatrices;
		WorldMatrices.reserve(GameObjects.second.size());

		CGameObject* pFirstGameObject = GameObjects.second[0];
		Safe_AddRef(pFirstGameObject);
		for (auto& pGameObject : GameObjects.second)
		{
			CTransform* pTransform = pGameObject->Get_Component<CTransform>(L"Com_Transform");
			WorldMatrices.push_back(pTransform->Get_WorldFloat4x4());

			Safe_Release(pGameObject);
		}

		if (FAILED(pFirstGameObject->Render(m_pVIBufferInstance, WorldMatrices)))
			return E_FAIL;

		Safe_Release(pFirstGameObject);
		GameObjects.second.clear();
	}

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BlurXY"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Blur"), "g_BlurTarget")))
		return E_FAIL;
		
	m_pShader->Begin(4);
	m_pVIBuffer->Render();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;



	// 백버퍼에 옮겨 그린다.
	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Blur_XY"), "g_BlurTarget")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(5)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BlurDownSample()
{
	return S_OK;
}

HRESULT CRenderer::Render_BlurXY()
{
	return S_OK;
}

HRESULT CRenderer::Render_BlurUpSample()
{
	return S_OK;
}

HRESULT CRenderer::Render_MRT_Final()
{

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto GameObjects : m_RenderObjects[RENDER_UI])
	{
		vector<_float4x4> WorldMatrices;
		WorldMatrices.reserve(GameObjects.second.size());

		CGameObject* pFirstGameObject = GameObjects.second[0];
		Safe_AddRef(pFirstGameObject);
		for (auto& pGameObject : GameObjects.second)
		{
			CTransform* pTransform = pGameObject->Get_Component<CTransform>(L"Com_Transform");
			WorldMatrices.push_back(pTransform->Get_WorldFloat4x4());

			Safe_Release(pGameObject);
		}

		if (FAILED(pFirstGameObject->Render(m_pVIBufferInstance, WorldMatrices)))
			return E_FAIL;

		Safe_Release(pFirstGameObject);
		GameObjects.second.clear();
	}

	return S_OK;
}

HRESULT CRenderer::Render_Debug()
{
	//for (auto& pDebugCom : m_RenderDebug)
	//{
	//	pDebugCom->Render();
	//	Safe_Release(pDebugCom);
	//}
	//m_RenderDebug.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Render(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Render(TEXT("MRT_Lights"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Render(TEXT("MRT_Shadow"), m_pShader, m_pVIBuffer)))
		return E_FAIL;


	if (FAILED(m_pTarget_Manager->Render(TEXT("MRT_Blur"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Render(TEXT("MRT_BlurXY"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

CRenderer * CRenderer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRenderer*	pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);

}

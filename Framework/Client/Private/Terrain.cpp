#include "stdafx.h"
#include "..\Public\Terrain.h"
#include "GameInstance.h"
#include "VIBuffer_Terrain.h"
#include "DebugDraw.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext, L"Terrain", OBJ_TYPE::OBJ_TERRAIN)
{

}

CTerrain::CTerrain(const CGameObject & rhs)
	: CGameObject(rhs)
{

}

HRESULT CTerrain::Initialize_Prototype()
{

#ifdef _DEBUG

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCodes = nullptr;
	size_t			iLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCodes, iLength, &m_pInputLayout)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	
	// m_pTransformCom->Set_Scale(XMVectorSet(1.f, 30.f, 30.f, 1.f));
	return S_OK;
}

void CTerrain::Priority_Tick(_float fTimeDelta)
{
	m_pNavigationCom->Update(m_pTransformCom->Get_WorldMatrix());

}

void CTerrain::Tick(_float fTimeDelta)
{
	
}

void CTerrain::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RENDER_NONBLEND, this);	
}

HRESULT CTerrain::Render()
{

#ifdef _DEBUG

	if (m_bDraw)
	{
		m_pEffect->SetWorld(m_pTransformCom->Get_WorldMatrix());
		m_pEffect->SetView(GAME_INSTANCE->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		m_pEffect->SetProjection(GAME_INSTANCE->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));


		m_pEffect->Apply(m_pContext);

		m_pContext->IASetInputLayout(m_pInputLayout);


		m_pBatch->Begin();

		_float3 vScale = m_pTransformCom->Get_Scale();
		_float4 fPosition;
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
		XMStoreFloat4(&fPosition, vPosition);

		DX::DrawGrid(m_pBatch,
			XMVectorSet(_float(m_pVIBufferCom->Get_VertexCount_X() / 2.f), 0.f, 0.f, 1.f),
			XMVectorSet(0.f, 0.f, _float(m_pVIBufferCom->Get_VertexCount_Z() / 2.f), 1.f),
			XMVectorSet((m_pVIBufferCom->Get_VertexCount_X() / 2.f), fPosition.y, (m_pVIBufferCom->Get_VertexCount_Z() / 2.f), 1.f),
			size_t((m_pVIBufferCom->Get_VertexCount_X()) * vScale.x),
			size_t((m_pVIBufferCom->Get_VertexCount_Z()) * vScale.z),
			DirectX::Colors::Orange);


		m_pBatch->End();
	}

	if (FAILED(m_pNavigationCom->Render()))
		return E_FAIL;
	
#endif

	return S_OK;
}


HRESULT CTerrain::Ready_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	///* Com_Texture*/
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
	//	TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	//	return E_FAIL;



	/* Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom)))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 던지자. */
	/*_float4x4 IdentityMatrix;
	XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &IdentityMatrix)))
		return E_FAIL;*/

	if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(GAME_INSTANCE->Bind_TransformToShader(m_pShaderCom, "g_ViewMatrix", CPipeLine::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformToShader(m_pShaderCom, "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;




	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTerrain*	pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrain::Clone(void* pArg)
{
	CTerrain*	pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

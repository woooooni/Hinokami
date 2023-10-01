#include "stdafx.h"
#include "..\Public\Terrain.h"
#include "GameInstance.h"
#include "VIBuffer_Terrain.h"
#include "DebugDraw.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext, L"Terrain")
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

void CTerrain::Tick(_float fTimeDelta)
{
	if (KEY_TAP(KEY::LBTN) && KEY_HOLD(KEY::CTRL))
	{
		_float4 vHitPos;
		Is_Picking(&vHitPos);

	}
}

void CTerrain::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RENDER_NONBLEND, this);	
}

HRESULT CTerrain::Render()
{

#ifdef _DEBUG


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
#endif

	return S_OK;
}

_bool CTerrain::Is_Picking(__out _float4* vHitPos)
{
	

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	_vector vMousePos = XMVectorSet(
		_float(pt.x / (g_iWinSizeX * .5f) - 1.f),
		_float(pt.y / (g_iWinSizeY * -.5f) + 1.f),
		0.f, 0.f);


	_matrix ViewMatrixInv = GAME_INSTANCE->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);
	_matrix ProjMatrixInv = GAME_INSTANCE->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_PROJ);

	vMousePos = XMVector3TransformCoord(vMousePos, ProjMatrixInv);

	XMVECTOR vRayDir, vRayPosition;

	vRayPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	vRayDir = vMousePos - vRayPosition;

	
	vMousePos = XMVector3TransformCoord(vRayPosition, ViewMatrixInv);
	vRayDir = XMVector3TransformNormal(vRayDir, ViewMatrixInv);


	_matrix WordlMatrixInv = m_pTransformCom->Get_WorldMatrixInverse();

	vRayPosition = XMVector3TransformCoord(vMousePos, WordlMatrixInv);
	vRayDir = XMVector3TransformNormal(vRayDir, WordlMatrixInv);

	vRayDir = XMVector3Normalize(vRayDir);

	_float fDistnace = 0.f;
	_float fMinDistance = 999999.f;


	const vector<VTXNORTEX>& vertices = m_pVIBufferCom->Get_Vertices();
	const vector<FACEINDICES32>& indices = m_pVIBufferCom->Get_Indices();



	for (_uint i = 0; i < m_pVIBufferCom->Get_PrimitiveCount(); ++i)
	{
		_vector v0, v1, v2;
		v0 = XMVectorSet(vertices[indices[i]._0].vPosition.x, vertices[indices[i]._0].vPosition.y, vertices[indices[i]._0].vPosition.z, 1.f);
		v1 = XMVectorSet(vertices[indices[i]._1].vPosition.x, vertices[indices[i]._1].vPosition.y, vertices[indices[i]._1].vPosition.z, 1.f);
		v2 = XMVectorSet(vertices[indices[i]._2].vPosition.x, vertices[indices[i]._2].vPosition.y, vertices[indices[i]._2].vPosition.z, 1.f);

		

		if (true == TriangleTests::Intersects(vRayPosition, vRayDir, v0, v1, v2, fDistnace))
		{
			if (fDistnace < fMinDistance)
			{
				XMStoreFloat4(vHitPos, vRayPosition + vRayDir * fDistnace);
				/*string strOut; 
				strOut += "x : " + to_string(vHitPos->x) + " ";
				strOut += "y : " + to_string(vHitPos->y) + " ";
				strOut += "z : " + to_string(vHitPos->z) + " ";
				MessageBoxA(g_hWnd, strOut.c_str(), "Hit", MB_OK);
				MSG_BOX("Hit");*/
				return true;
			}
		}
	}

	return false;
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

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
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

	//if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
	//	return E_FAIL;

	;




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

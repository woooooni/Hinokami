#include "..\Public\Trail.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "GameInstance.h"
#include "GameObject.h"


CTrail::CTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CTrail::CTrail(const CTrail & rhs)
	: CVIBuffer(rhs)
	
{

}

HRESULT CTrail::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CTrail::Initialize(void * pArg)
{

	if (pArg == nullptr)
		return E_FAIL;

	TRAIL_DESC* pDesc = (TRAIL_DESC*)pArg;
	memcpy(&m_tTrailDesc, pDesc, sizeof(TRAIL_DESC));

#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 400;
	m_iStride = sizeof(VTXPOSTEX);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC/*D3D11_USAGE_DEFAULT*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE/*0*/;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];


	for (_uint i = 0; i < 400; i += 2)
	{
		pVertices[i].vPosition = _float3(0.f, 0.f, 0.f);
		pVertices[i].vTexcoord = _float2(0.f, 0.f);

		pVertices[i + 1].vPosition = _float3(0.f, 0.f, 0.f);
		pVertices[i + 1].vTexcoord = _float2(0.f, 0.f);
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;


	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = 398;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES16);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	FACEINDICES16* pIndices = new FACEINDICES16[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitives);

	for (_uint i = 0; i < m_iNumPrimitives; i += 2)
	{
		pIndices[i]._0 = i + 3;
		pIndices[i]._1 = i + 1;
		pIndices[i]._2 = i;

		pIndices[i + 1]._0 = i + 2;
		pIndices[i + 1]._1 = i + 3;
		pIndices[i + 1]._2 = i;
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion


	m_pTextureCom = dynamic_cast<CTexture*>(GI->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Texture_Trail"));
	if (nullptr == m_pTextureCom)
		return E_FAIL;

	m_pShaderCom = dynamic_cast<CShader*>(GI->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Shader_Trail"));
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

void CTrail::Tick(_float fTimedelta, _fmatrix TransformMatrix)
{
	m_tTrailDesc.fAccGenTrail += fTimedelta;
	vector<VTXPOSTEX> Vertices;
	Vertices.reserve(m_iNumVertices);

	if (m_tTrailDesc.fAccGenTrail >= m_tTrailDesc.fGenTrailTime)
	{
		m_tTrailDesc.fAccGenTrail = 0.f;
		m_tTrailDesc.fUVAcc += fTimedelta;

		if (m_tTrailDesc.fUVAcc >= 1.f)
			m_tTrailDesc.fUVAcc = 0.f;

		while (m_TrailVertices.size() > m_iNumVertices - 2)
			m_TrailVertices.pop_front();
		

		_vector vHigh = XMVector3TransformCoord(XMLoadFloat4(&m_vHighPosition), TransformMatrix);
		_vector vLow = XMVector3TransformCoord(XMLoadFloat4(&m_vLowPosition), TransformMatrix);


		_vector vCameraPosition = XMLoadFloat4(&GI->Get_CamPosition());
		_float fAngle = atan2(XMVectorGetX(vHigh) - XMVectorGetX(vCameraPosition), XMVectorGetZ(vHigh) - XMVectorGetZ(vCameraPosition)) * (180.0 / 3.141592);
		fAngle = XMConvertToRadians(fAngle);


		VTXPOSTEX Vertex = {};
		XMStoreFloat3(&Vertex.vPosition, vHigh);
		m_TrailVertices.push_back(Vertex);

		XMStoreFloat3(&Vertex.vPosition, vLow);
		m_TrailVertices.push_back(Vertex);

		for (_uint i = 0; i < m_TrailVertices.size(); i += 2)
		{
			_float _iVtxCount = 0.f;
			if (m_TrailVertices.size() < 1)
				_iVtxCount = 1.f;
			else
				_iVtxCount = m_TrailVertices.size();
				
			m_TrailVertices[i].vTexcoord = { ((_float)i) / ((_float)_iVtxCount), 0.f };
			m_TrailVertices[i + 1].vTexcoord = { ((_float)i) / ((_float)_iVtxCount), 1.f };

			Vertices.push_back(m_TrailVertices[i]);
			Vertices.push_back(m_TrailVertices[i + 1]);
		}
	}
	

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, Vertices.data(), sizeof(VTXPOSTEX) * Vertices.size());
	m_pContext->Unmap(m_pVB, 0);
}

void CTrail::Late_Tick(_float fTimeDelta)
{
	
}

HRESULT CTrail::Render()
{
	_float4x4 MatIdentity;
	XMStoreFloat4x4(&MatIdentity, XMMatrixTranspose(XMMatrixIdentity()));

	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &MatIdentity, sizeof(_float4x4))))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewMatrix", &GI->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GI->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (m_pShaderCom->Bind_RawValue("g_TrailColor", &m_tTrailDesc.vColor, sizeof(_float4)))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	__super::Render();

	return S_OK;
}

void CTrail::SetUp_Position(_vector vHighPosition, _vector vLowPosition)
{
	XMStoreFloat4(&m_vHighPosition, vHighPosition);
	XMStoreFloat4(&m_vLowPosition, vLowPosition);
	
}

void CTrail::Start_Trail(_matrix TransformMatrix)
{
	m_TrailVertices.clear();
	for (_uint i = 0; i < m_iNumVertices; i += 2)
	{
		VTXPOSTEX High = {};
		XMStoreFloat3(&High.vPosition, XMVector3TransformCoord(XMLoadFloat4(&m_vHighPosition), TransformMatrix));
		VTXPOSTEX Low = {};
		XMStoreFloat3(&Low.vPosition, XMVector3TransformCoord(XMLoadFloat4(&m_vLowPosition), TransformMatrix));

		m_TrailVertices.push_back(High);
		m_TrailVertices.push_back(Low);
	}

	m_tTrailDesc.bTrail = true;
}

void CTrail::Stop_Trail()
{
	m_TrailVertices.clear();

}

CTrail * CTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTrail*			pInstance = new CTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTrail::Clone(void * pArg)
{
	CTrail*			pInstance = new CTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail::Free()
{
	__super::Free();
}
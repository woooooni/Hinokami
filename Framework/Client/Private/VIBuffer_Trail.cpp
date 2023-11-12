#include "..\Public\VIBuffer_Trail.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "GameInstance.h"
#include "GameObject.h"


CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iVtxCount)
	: CVIBuffer(pDevice, pContext)
{
	m_iVtxCount = iVtxCount;
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail & rhs)
	: CVIBuffer(rhs)
	, m_iVtxCount(rhs.m_iVtxCount)
{

}

HRESULT CVIBuffer_Trail::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void * pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 100000;
	m_iStride = sizeof(VTXPOSTEX);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC/*D3D11_USAGE_DEFAULT*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE/*0*/;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];


	for (_uint i = 0; i < 100000; i += 2)
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
	m_iNumPrimitives = m_iNumVertices - 2;
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


	return S_OK;
}

void CVIBuffer_Trail::Update_TrailBuffer(_float fTimedelta, _matrix TransformMatrix)
{
	
	vector<VTXPOSTEX> Vertices;
	Vertices.reserve(m_iNumVertices);

	TransformMatrix.r[CTransform::STATE_RIGHT] = XMVector4Normalize(TransformMatrix.r[CTransform::STATE_RIGHT]);
	TransformMatrix.r[CTransform::STATE_UP] = XMVector4Normalize(TransformMatrix.r[CTransform::STATE_UP]);
	TransformMatrix.r[CTransform::STATE_LOOK] = XMVector4Normalize(TransformMatrix.r[CTransform::STATE_LOOK]);

	_vector vWorldHigh = XMVector3TransformCoord(XMLoadFloat4(&m_vHighPosition), TransformMatrix);
	_vector vWorldLow = XMVector3TransformCoord(XMLoadFloat4(&m_vLowPosition), TransformMatrix);


	while (m_TrailVertices.size() < m_iVtxCount)
	{
		VTXPOSTEX Vertex = {};
		XMStoreFloat3(&Vertex.vPosition, vWorldHigh);
		m_TrailVertices.push_back(Vertex);

		XMStoreFloat3(&Vertex.vPosition, vWorldLow);
		m_TrailVertices.push_back(Vertex);
	}


	while (m_iVtxCount - 2 < m_TrailVertices.size())
		m_TrailVertices.pop_front();

	VTXPOSTEX Vertex = {};
	XMStoreFloat3(&Vertex.vPosition, vWorldHigh);
	m_TrailVertices.push_back(Vertex);

	XMStoreFloat3(&Vertex.vPosition, vWorldLow);
	m_TrailVertices.push_back(Vertex);

	

	for (_uint i = 0; i < m_TrailVertices.size(); i++)
	{
		_float _iVtxCount = _float(m_TrailVertices.size());

		if (i % 2 == 0)
			m_TrailVertices[i].vTexcoord = { ((_float)i) / ((_float)_iVtxCount), 0.f };
		else		
			m_TrailVertices[i].vTexcoord = { ((_float)i) / ((_float)_iVtxCount), 1.f };
		Vertices.push_back(m_TrailVertices[i]);
	}

	vector<VTXPOSTEX> CatmallomVertices;
	CatmallomVertices.reserve((m_iVtxCount - 2) * 10);
	for (_uint i = 0; i < Vertices.size(); i += 2)
	{
		if (i >= Vertices.size() - 4)
		{
			CatmallomVertices.push_back(Vertices[i]);
			CatmallomVertices.push_back(Vertices[i + 1]);
			continue;
		}

		VTXPOSTEX vHigh0, vHigh1, vHigh2, vHigh3;
		VTXPOSTEX vLow0, vLow1, vLow2, vLow3;

		if (i == 0)
		{
			vHigh0 = Vertices[i];
			vLow0 = Vertices[i + 1];
		}
		else
		{
			vHigh0 = Vertices[i - 2];
			vLow0 = Vertices[i - 1];
		}
		
		vHigh1 = Vertices[i];
		vHigh2 = Vertices[i + 2];
		vHigh3 = Vertices[i + 4];


		vLow1 = Vertices[i + 1];
		vLow2 = Vertices[i + 3];
		vLow3 = Vertices[i + 5];

		for (_uint j = 0; j < 10; ++j)
		{
			VTXPOSTEX VertexHigh = {};
			XMStoreFloat3(&VertexHigh.vPosition, XMVectorCatmullRom(
				XMLoadFloat3(&vHigh0.vPosition)
				, XMLoadFloat3(&vHigh1.vPosition)
				, XMLoadFloat3(&vHigh2.vPosition)
				, XMLoadFloat3(&vHigh3.vPosition)
				, j * 0.1f));

			XMStoreFloat2(&VertexHigh.vTexcoord, XMVectorCatmullRom(
				XMLoadFloat2(&vHigh0.vTexcoord)
				, XMLoadFloat2(&vHigh1.vTexcoord)
				, XMLoadFloat2(&vHigh2.vTexcoord)
				, XMLoadFloat2(&vHigh3.vTexcoord)
				, j * 0.1f));

			CatmallomVertices.push_back(VertexHigh);


			VTXPOSTEX VertexLow = {};
			XMStoreFloat3(&VertexLow.vPosition, XMVectorCatmullRom(
				XMLoadFloat3(&vLow0.vPosition)
				, XMLoadFloat3(&vLow1.vPosition)
				, XMLoadFloat3(&vLow2.vPosition)
				, XMLoadFloat3(&vLow3.vPosition)
				, j * 0.1f));

			XMStoreFloat2(&VertexLow.vTexcoord, XMVectorCatmullRom(
				XMLoadFloat2(&vLow0.vTexcoord)
				, XMLoadFloat2(&vLow1.vTexcoord)
				, XMLoadFloat2(&vLow2.vTexcoord)
				, XMLoadFloat2(&vLow3.vTexcoord)
				, j * 0.1f));

			CatmallomVertices.push_back(VertexLow);
		}
		
	}
	

	m_iNumPrimitives = CatmallomVertices.size() - 2;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, CatmallomVertices.data(), sizeof(VTXPOSTEX) * CatmallomVertices.size());
	m_pContext->Unmap(m_pVB, 0);
}



HRESULT CVIBuffer_Trail::Render()
{
	__super::Render();

	return S_OK;
}

void CVIBuffer_Trail::SetUp_Position(_vector vHighPosition, _vector vLowPosition)
{
	XMStoreFloat4(&m_vHighPosition, vHighPosition);
	XMStoreFloat4(&m_vLowPosition, vLowPosition);
	
}

void CVIBuffer_Trail::Start_Trail(_matrix TransformMatrix)
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
}

void CVIBuffer_Trail::Stop_Trail()
{	

	m_TrailVertices.clear();
}

CVIBuffer_Trail * CVIBuffer_Trail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iVtxCount)
{
	CVIBuffer_Trail*			pInstance = new CVIBuffer_Trail(pDevice, pContext, iVtxCount);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Trail::Clone(void * pArg)
{
	CVIBuffer_Trail*			pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();
}

#include "..\Public\VIBuffer_Instancing.h"
#include "Mesh.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{

}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs)
	: CVIBuffer(rhs)
	, m_iStrideInstance(rhs.m_iStrideInstance)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_pVertices(rhs.m_pVertices)
{

}

HRESULT CVIBuffer_Instancing::Initialize_Prototype()
{
	m_iNumVertexBuffers = 2;
	m_iStrideInstance = sizeof(VTXINSTANCE);

	/* 정점버퍼와 인덱스 버퍼를 만드낟. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStrideInstance * 2000;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStrideInstance;

	m_pVertices = new VTXINSTANCE[2000];
	ZeroMemory(m_pVertices, sizeof(VTXINSTANCE) * 2000);


	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = m_pVertices;


	if (FAILED(__super::Create_Buffer(&m_pVBInstance)))
		return E_FAIL;


	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render(const vector<_float4x4>& WorldMatrices, CVIBuffer* pVIBuffer)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource = {};

	m_pVB = pVIBuffer->Get_Vertex_Buffer();
	m_pIB = pVIBuffer->Get_Index_Buffer();


	m_iStride = pVIBuffer->Get_Stride();
	m_eIndexFormat = pVIBuffer->Get_IndexFormat();
	m_eTopology = pVIBuffer->Get_Topology();
	m_iNumIndicesofPrimitive = pVIBuffer->Get_IndicesOfPrimitive();
	m_iNumPrimitives = pVIBuffer->Get_NumPrimitives();

	m_iNumInstance = WorldMatrices.size();

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	memcpy(SubResource.pData, WorldMatrices.data(), sizeof(_float4x4) * m_iNumInstance);
	m_pContext->Unmap(m_pVBInstance, 0);

	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint			iStrides[] = {
		m_iStride,
		m_iStrideInstance,
	};

	_uint			iOffsets[] = {
		0,
		0,
	};

	/* 버텍스 버퍼들을 할당한다. */
	/* 그리기용 정점버퍼 + 상태변환용 정점버퍼 */
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	/* 인덱스 버퍼를 할당한다. */
	/* 그리고자 하는 인스턴스의 갯수만큼 확대되어있는 인덱스 버퍼. */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	/* 해당 정점들을 어떤 방식으로 그릴꺼야. */
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	/* 인덱스가 가르키는 정점을 활용하여 그린다. */
	m_pContext->DrawIndexedInstanced(m_iNumPrimitives * m_iNumIndicesofPrimitive, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

CVIBuffer_Instancing* CVIBuffer_Instancing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Instancing* pInstance = new CVIBuffer_Instancing(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Create Failed : CVIBuffer_Instancing");
		return nullptr;
	}

	return pInstance;
}

CComponent* CVIBuffer_Instancing::Clone(void* pArg)
{
	CVIBuffer_Instancing* pInstance = new CVIBuffer_Instancing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Clone Failed : CVIBuffer_Instancing");
		return nullptr;
	}

	return pInstance;
}

void CVIBuffer_Instancing::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pVertices);
	}

	Safe_Release(m_pVBInstance);
}
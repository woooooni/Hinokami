#include "..\Public\VIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{

}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing & rhs)
	: CVIBuffer(rhs)
	, m_iStrideInstance(rhs.m_iStrideInstance)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_iNumIndicesPerInstance(rhs.m_iNumIndicesPerInstance)	
	, m_pVertices(rhs.m_pVertices)
{

}

HRESULT CVIBuffer_Instancing::Initialize_Prototype(const INSTANCE_DESC& InstanceDesc)
{
	m_iNumInstance = InstanceDesc.iNumInstance;
	m_iStrideInstance = sizeof(VTXINSTANCE);

	/* �������ۿ� �ε��� ���۸� ���峮. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	// m_BufferDesc.ByteWidth = �����ϳ��� ũ��(Byte) * ������ ����;
	m_BufferDesc.ByteWidth = m_iStrideInstance * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStrideInstance;

	m_pVertices = new VTXINSTANCE[m_iNumInstance];
	ZeroMemory(m_pVertices, sizeof(VTXINSTANCE) * m_iNumInstance);


	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = m_pVertices;



	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void * pArg)
{
	if (FAILED(__super::Create_Buffer(&m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
	ID3D11Buffer*	pVertexBuffers[] = {
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

	/* ���ؽ� ���۵��� �Ҵ��Ѵ�. */
	/* �׸���� �������� + ���º�ȯ�� �������� */
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	/* �ε��� ���۸� �Ҵ��Ѵ�. */
	/* �׸����� �ϴ� �ν��Ͻ��� ������ŭ Ȯ��Ǿ��ִ� �ε��� ����. */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	/* �ش� �������� � ������� �׸�����. */
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	/* �ε����� ����Ű�� ������ Ȱ���Ͽ� �׸���. */
	m_pContext->DrawIndexedInstanced(m_iNumIndicesPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
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

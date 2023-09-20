#include "..\Public\PipeLine.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

void CPipeLine::Set_Transform(TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrix[eTransformState], TransformMatrix);
}

void CPipeLine::Tick()
{
	for (_uint i = 0; i < D3DTS_END; ++i)	
		XMStoreFloat4x4(&m_TransformInverseMatrix[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[i])));	

	memcpy(&m_vCamPosition, &m_TransformInverseMatrix[D3DTS_VIEW].m[3][0], sizeof(_float4));
}

HRESULT CPipeLine::Bind_TransformToShader(CShader* pShader, const char* pConstantName, CPipeLine::TRANSFORMSTATE eState)
{
	return pShader->Bind_Matrix(pConstantName, &m_TransformMatrix[eState]);
}

void CPipeLine::Free()
{
	__super::Free();
}

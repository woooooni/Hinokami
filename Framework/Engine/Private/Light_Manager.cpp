#include "..\Public\Light_Manager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
}

const LIGHTDESC * CLight_Manager::Get_LightDesc(_uint iIndex)
{
	auto	iter = m_Lights.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightDesc();	
}

HRESULT CLight_Manager::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	CLight*		pLight = CLight::Create(pDevice, pContext, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Add_ShadowLight(_uint iLevelIndex, _vector vEye, _vector vAt, _vector vUp)
{
	auto iter = m_ShadowWorldMatrix.find(iLevelIndex);

	if (iter != m_ShadowWorldMatrix.end())
		return E_FAIL;

	_float4x4 LightViewMatrix; 
	XMStoreFloat4x4(&LightViewMatrix, XMMatrixLookAtLH(vEye, vAt, vUp));

	m_ShadowWorldMatrix.emplace(iLevelIndex, LightViewMatrix);

	return S_OK;
}

HRESULT CLight_Manager::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	for (auto& pLight : m_Lights)
		pLight->Render(pShader, pVIBuffer);

	return S_OK;
}

_float4x4 CLight_Manager::Get_ShadowLightViewMatrix(_uint iLevelIndex)
{	
	_float4x4 IdentityMatrix;  
	XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());
	auto iter = m_ShadowWorldMatrix.find(iLevelIndex);

	if (iter == m_ShadowWorldMatrix.end())
		return IdentityMatrix;

	return iter->second;
}

_float4x4 CLight_Manager::Get_ShadowLightViewMatrix_Inverse(_uint iLevelIndex)
{
	_float4x4 LightViewMatrix;
	XMStoreFloat4x4(&LightViewMatrix, XMMatrixIdentity());
	auto iter = m_ShadowWorldMatrix.find(iLevelIndex);

	if (iter == m_ShadowWorldMatrix.end())
		return LightViewMatrix;

	XMStoreFloat4x4(&LightViewMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&iter->second)));
	return LightViewMatrix;
}

HRESULT CLight_Manager::Reset_Lights()
{
	Free();
	return S_OK;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}

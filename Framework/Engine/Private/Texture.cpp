#include "..\Public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs)
	, m_iNumTextures(rhs.m_iNumTextures)
	, m_SRVs(rhs.m_SRVs)
{
	for (auto& pSRV : m_SRVs)
		Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;
	m_FileNames.reserve(m_iNumTextures);

	for (_uint i = 0; i < iNumTextures; ++i)
	{
		_tchar			szFullPath[MAX_PATH] = TEXT("");

		wsprintf(szFullPath, strTextureFilePath.c_str(), i);

		_tchar			szDrive[MAX_PATH] = TEXT("");
		_tchar			szName[MAX_PATH] = TEXT("");
		_tchar			szExt[MAX_PATH] = TEXT("");

		_wsplitpath_s(szFullPath, szDrive, MAX_PATH, nullptr, 0, szName, MAX_PATH, szExt, MAX_PATH);

		HRESULT hr = 0;

		ID3D11ShaderResourceView* pSRV = nullptr;

		if (false == lstrcmp(TEXT(".dds"), szExt))
			hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);

		else if (false == lstrcmp(TEXT(".tga"), szExt))
			hr = E_FAIL;

		else
			hr = DirectX::CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);

		if (FAILED(hr))
			return E_FAIL;

		m_FileNames.push_back(wstring(szName) + szExt);
		m_SRVs.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(const CShader* pShader, const char* pConstantName, _uint iTextureIndex)
{
	return pShader->Bind_Texture(pConstantName, m_SRVs[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResources(const CShader* pShader, const char* pConstantName)
{
	return pShader->Bind_Textures(pConstantName, m_SRVs.data(), m_iNumTextures);
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed To Created : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_SRVs)
		Safe_Release(pSRV);

	m_SRVs.clear();
}

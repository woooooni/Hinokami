#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring & strTextureFilePath, _uint iNumTextures, _bool bWithPath);
	virtual HRESULT Initialize(void* pArg);

public:
	/* 테긋쳐 한장을 쉐이더로 던진다. */
	HRESULT Bind_ShaderResource(const class CShader* pShader, const char* pConstantName, _uint iTextureIndex = 0);

	/* 벡터에 추가해놓은 모든 텍스쳐를 셰이더 배열에 던진다.  */
	HRESULT Bind_ShaderResources(const class CShader* pShader, const char* pConstantName);


public:
	const wstring& Get_Name() { return m_FileNames[0]; }
	const wstring& Get_Name(_uint iIdx) { 
		if (iIdx >= m_FileNames.size())
			return wstring();
		return m_FileNames[iIdx];
	}

	_int Find_Index(const wstring& strTextureName)
	{
		_uint iTextureIndex = 0;
		for (wstring strName : m_FileNames)
		{
			if (strName == strTextureName)
				return iTextureIndex;

			iTextureIndex++; 
		}
		return -1;
	}

	_uint Get_TextureCount() { return m_iNumTextures; }
	ID3D11ShaderResourceView* Get_Srv(_uint iIdx) 
	{
		if (iIdx >= m_SRVs.size())
			return nullptr;
		return m_SRVs[iIdx];
	};


private:
	HRESULT Load_Texture(const wstring& strTextureFilePath, _uint iNumTextures);
	HRESULT Load_Texture_In_Path(const wstring& strTextureFilePath);

private:
	/* 셰이더에 직접 바인딩 될 수 있는 객체 */
	vector<ID3D11ShaderResourceView*>				m_SRVs;
	typedef vector<ID3D11ShaderResourceView*>		SRVS;

	_uint			m_iNumTextures = 0;
	vector<wstring>	m_FileNames;

public:
	static CTexture* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strTextureFilePath, _uint iNumTextures = 1, _bool bWithPath = false);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();
};

END
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
	virtual HRESULT Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg);

public:
	/* �ױ��� ������ ���̴��� ������. */
	HRESULT Bind_ShaderResource(const class CShader* pShader, const char* pConstantName, _uint iTextureIndex = 0);

	/* ���Ϳ� �߰��س��� ��� �ؽ��ĸ� ���̴� �迭�� ������.  */
	HRESULT Bind_ShaderResources(const class CShader* pShader, const char* pConstantName);


public:
	const wstring& Get_Name() { return m_FileNames[0]; }
	const wstring& Get_Name(_uint iIdx) { 
		if (iIdx >= m_FileNames.size())
			return wstring();
		return m_FileNames[iIdx];
	}
	_uint Get_TextureCount() { return m_iNumTextures; }

private:
	/* ���̴��� ���� ���ε� �� �� �ִ� ��ü */
	vector<ID3D11ShaderResourceView*>				m_SRVs;
	typedef vector<ID3D11ShaderResourceView*>		SRVS;

	_uint			m_iNumTextures = 0;
	vector<wstring>	m_FileNames;

public:
	static CTexture* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();
};

END
#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
public:
	typedef struct tagPassDesc
	{
		ID3DX11EffectPass* pPass = nullptr;
		ID3D11InputLayout* pInputLayout = nullptr;
	}PASSDESC;
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements);
	virtual HRESULT Initialize(void* pArg);

public:
	/* ���̴� ���������� �����͸� ������. */
	HRESULT Bind_RawValue(const char* pConstantName, const void* pData, _uint iByteLength) const;
	HRESULT Bind_Matrix(const char* pConstantName, const _float4x4* pMatrix) const;
	HRESULT Bind_Matrices(const char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices)const;
	HRESULT Bind_Texture(const char* pConstantName, ID3D11ShaderResourceView* pSRV)const;
	HRESULT Bind_Textures(const char* pConstantName, ID3D11ShaderResourceView** ppSRVs, _uint iNumTextures)const;

public: /* �� ���̴��� Ư�� �н��� �׸���. */
	HRESULT Begin(_uint iPassIndex);

private:
	ID3DX11Effect* m_pEffect = nullptr;
	vector<PASSDESC>			m_Passes;

public:
	static CShader* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END
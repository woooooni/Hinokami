#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int			iCurrentIndex = { -1 };
	} NAVIGATION_DESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring & strNavigationDataFiles);
	virtual HRESULT Initialize(void* pArg) override;	
	void Update(_fmatrix WorldMatrix);

	_bool Is_Movable(_fvector vPoint);
	_float Compute_Height(_vector vPosition);

public:
	HRESULT Create_Cell(const _float3* pPoints);
	HRESULT Delete_Cell(const _fvector vMouseWorldDir, const _fvector vMouseWorldPosition);
	HRESULT Clear_Cells();

	HRESULT Save_NaviData(const wstring& strNaviDataPath);
	HRESULT Load_NaviData(const wstring& strNaviDataPath);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	static _float4x4 m_WorldIdentity;
	_int							m_iCurrentIndex = { -1 };
	vector<class CCell*>			m_Cells;

private:
	int Compute_CW(_float3 vPointA, _float3 vPointB, _float3 vPointC);

#ifdef _DEBUG
private:
	class CShader*					m_pShader = { nullptr };
#endif

public:
	HRESULT SetUp_Neighbors();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strNavigationDataFiles);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
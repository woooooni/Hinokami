#pragma once

#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
public:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHTDESC* Get_LightDesc(_uint iIndex);

public:
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	HRESULT Add_ShadowLight(_uint iLevelIndex, _vector vEye, _vector vAt, _vector vUp);

	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);


public:
	_float4x4 Get_ShadowLightViewMatrix(_uint iLevelIndex);
	_float4x4 Get_ShadowLightViewMatrix_Inverse(_uint iLevelIndex);

public:
	HRESULT Reset_Lights();

private:
	list<class CLight*>			m_Lights;
	typedef list<class CLight*>	LIGHTS;


private:
	map<_uint, _float4x4> m_ShadowWorldMatrix;


public:
	virtual void Free() override;
};

END
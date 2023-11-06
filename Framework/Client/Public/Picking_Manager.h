#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CNavigation;
class CTransform;
class CVIBuffer;

END

BEGIN(Client)

class CPicking_Manager : public CBase
{
	DECLARE_SINGLETON(CPicking_Manager)

private:
	CPicking_Manager();
	virtual ~CPicking_Manager() = default;

public:
	HRESULT Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Tick(_float fTimeDelta);

public:
	_bool Is_Picking(class CTransform* pTransform, class CVIBuffer* pBuffer, _bool bCutPos, __out _float4* vWorldOut);
	_bool Is_NaviPicking(class CTransform* pTransform, class CVIBuffer* pBuffer, __out _float3* pWorldOut = nullptr, __out _float3* pLocalPos = nullptr);
	_bool Is_JustNaviPicking(class CNavigation* pNavigation, __out _float3* pWorldOut);

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;


public:
	virtual void Free() override;
};

END
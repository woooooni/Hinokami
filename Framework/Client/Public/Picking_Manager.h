#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer;
END

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
	_bool Is_Picking(class CTransform* pTransform, class CVIBuffer* pBuffer, _bool bCutPos, __out _float4* vOut);

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;


public:
	virtual void Free() override;
};


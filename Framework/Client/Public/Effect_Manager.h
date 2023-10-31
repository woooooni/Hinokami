#pragma once

#include "Client_Defines.h"
#include "Pool.h"
#include "Base.h"

BEGIN(Engine)
class CEffect;
END

class CEffect_Manager : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager)

private:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;

public:
	HRESULT Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Tick(_float fTimeDelta);

public:
	HRESULT Generate_Effect(const wstring& strPrototypeEffectName, _matrix TransformMatrix);

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;


public:
	virtual void Free() override;
};


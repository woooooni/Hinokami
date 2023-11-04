#pragma once

#include "Client_Defines.h"
#include "Pool.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CParticle_Manager : public CBase
{
	DECLARE_SINGLETON(CParticle_Manager)

private:
	CParticle_Manager();
	virtual ~CParticle_Manager() = default;

public:
	HRESULT Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPatriclePath);
	void	Tick(_float fTimeDelta);

public:
	HRESULT Generate_Particle(const wstring& strPrototypeParticleName, _matrix TransformMatrix);




private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;


private:
	HRESULT Ready_Proto_Particles(const wstring& strParticlePath);


public:
	virtual void Free() override;
};

END

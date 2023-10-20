#pragma once
#include "Character.h"

BEGIN(Client)
class CZenitsu final : public CCharacter
{

private:
	CZenitsu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	CZenitsu(const CZenitsu& rhs);
	virtual ~CZenitsu() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Ready_States() override;

private:
	HRESULT Ready_Sockets();
	HRESULT Ready_Parts();

public:
	static CZenitsu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


#pragma once
#include "Character.h"

BEGIN(Client)
class CTanjiro final : public CCharacter
{

private:
	CTanjiro(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	CTanjiro(const CTanjiro& rhs);
	virtual ~CTanjiro() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	class CSword* Get_Sword() { return m_pSword; }
	class CSweath* Get_Sweath() { return m_pSweath; }

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Ready_States() override;


private:
	class CSword* m_pSword = nullptr;
	class CSweath* m_pSweath = nullptr;

private:
	HRESULT Ready_Sockets();
	HRESULT Ready_Parts();

public:
	static CTanjiro* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


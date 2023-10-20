#pragma once

#include "Client_Defines.h"
#include "GameObject.h"



BEGIN(Client)
class CPlayer final : public CGameObject
{

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Set_MainCharacter(class CCharacter* pCharacter);
	class CCharacter* Get_CurrCharacter() { return m_pCurrCharacter; }
	HRESULT Add_Character(class CCharacter* pCharacter) { m_Characters.push_back(pCharacter); }

protected:
	virtual HRESULT Ready_Components() override;
	

private:
	vector<class CCharacter*> m_Characters;
	class CCharacter* m_pCurrCharacter = nullptr;

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

	
};

END


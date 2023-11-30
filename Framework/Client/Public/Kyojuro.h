#pragma once
#include "Character.h"

BEGIN(Client)
class CKyojuro final : public CCharacter
{

private:
	CKyojuro(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, CCharacter::CHARACTER_TYPE eCharacterType);
	CKyojuro(const CKyojuro& rhs);
	virtual ~CKyojuro() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Collision_Enter(const COLLISION_INFO& tInfo) override;
	virtual void Collision_Continue(const COLLISION_INFO& tInfo) override;
	virtual void Collision_Exit(const COLLISION_INFO& tInfo) override;


public:
	virtual void On_Damaged(const COLLISION_INFO& tInfo) override;


protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Ready_States() override;
	virtual HRESULT Ready_Colliders() override;

private:
	HRESULT Ready_Sockets();
	HRESULT Ready_Parts();


public:
	static CKyojuro* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, CCharacter::CHARACTER_TYPE eCharacterType);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


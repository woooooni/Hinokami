#pragma once
#include "Monster.h"

BEGIN(Client)
class CBoss_Enmu final : public CMonster
{

private:
	CBoss_Enmu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const MONSTER_STAT& tStat);
	CBoss_Enmu(const CBoss_Enmu& rhs);
	virtual ~CBoss_Enmu() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Enter_Scene() override;
	virtual void Return_Pool() override;

public:
	virtual void Collision_Enter(const COLLISION_INFO& tInfo) override;
	virtual void Collision_Continue(const COLLISION_INFO& tInfo) override;
	virtual void Collision_Exit(const COLLISION_INFO& tInfo) override;
	virtual void On_Damaged(const COLLISION_INFO& tInfo) override;

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Ready_States() override;
	virtual HRESULT Ready_Colliders() override;


private:
	_int m_iSkillCount = 3;
	
	

private:
	HRESULT Ready_Sockets();
	HRESULT Ready_Parts();
	

public:
	static CBoss_Enmu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const MONSTER_STAT& tStat);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


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
	virtual HRESULT Render(class CVIBuffer_Instancing* pBufferInstance, const vector<_float4x4>& WorldMatrices) override;

public:
	virtual void Collision_Enter(const COLLISION_INFO& tInfo) override;
	virtual void Collision_Continue(const COLLISION_INFO& tInfo) override;
	virtual void Collision_Exit(const COLLISION_INFO& tInfo) override;


public:
	virtual void On_Damaged(CGameObject* pAttacker) override;
	

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Ready_States() override;
	virtual HRESULT Ready_Colliders() override;

private:
	HRESULT Ready_Sockets();
	HRESULT Ready_Parts();
	

public:
	static CTanjiro* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


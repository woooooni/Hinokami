#include "stdafx.h"
#include "CCharacter.h"


USING(Client)
CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, wstring strObjectTag)
	: CGameObject(pDevice, pContext, strObjectTag)
{
}

CCharacter::CCharacter(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCharacter::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CCharacter::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}

void CCharacter::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CCharacter::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CCharacter::Render()
{
	__super::Render();
	return S_OK;
}

void CCharacter::Free()
{
	__super::Free();
}

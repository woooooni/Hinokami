#include "stdafx.h"
#include "GameInstance.h"
#include "Player.h"
#include "HierarchyNode.h"
#include "Key_Manager.h"
#include "Character.h"
#include "Tanjiro.h"


CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext, L"Player", OBJ_TYPE::OBJ_PLAYER)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
	, m_Characters(rhs.m_Characters)
	, m_pCurrCharacter(rhs.m_pCurrCharacter)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{

	CGameObject* pGameObject = GAME_INSTANCE->Clone_GameObject(L"Prototype_GameObject_Giyu", LAYER_TYPE::LAYER_CHARACTER);
	if (nullptr == pGameObject)
		return E_FAIL;

	CCharacter* pCharacter = dynamic_cast<CCharacter*>(pGameObject);
	if (nullptr == pCharacter)
	{
		Safe_Release(pGameObject);
		return E_FAIL;
	}

	m_Characters.push_back(pCharacter);
	Set_MainCharacter(pCharacter);
	
    return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < m_Characters.size(); i++)
		m_Characters[i]->Tick(fTimeDelta);
}

void CPlayer::LateTick(_float fTimeDelta)
{
	for (size_t i = 0; i < m_Characters.size(); i++)
		m_Characters[i]->LateTick(fTimeDelta);
}

HRESULT CPlayer::Render()
{
	if(FAILED(m_pCurrCharacter->Render()))
		return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Set_MainCharacter(CCharacter* pCharacter)
{
	//if (nullptr == pCharacter)
	//	return E_FAIL;

	//if (m_pCurrCharacter)
	//{
	//	m_pCurrCharacter->Set_Controlable(false);
	//	m_pCurrCharacter->Set_MainCharacter(false);
	//}

	//m_pCurrCharacter = pCharacter;

	//m_pCurrCharacter->Set_Controlable(true);
	//m_pCurrCharacter->Set_MainCharacter(true);

	//return S_OK;
	return S_OK;
}





CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();
	for (size_t i = 0; i < m_Characters.size(); i++)
		Safe_Release(m_Characters[i]);
}

HRESULT CPlayer::Ready_Components()
{
	return S_OK;
}

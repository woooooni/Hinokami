#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	WRITE_LOCK
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Priority_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Priority_Tick(fTimeDelta);
	}
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Tick(fTimeDelta);
	}
}

void CLayer::LateTick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->LateTick(fTimeDelta);
	}
}

void CLayer::Clear()
{
	for (auto& iter : m_GameObjects)
		Safe_Release(iter);
}

CGameObject* CLayer::Find_GameObject(const wstring& strObjectTag)
{
	READ_LOCK
	CGameObject* pObj = nullptr;
	auto iter = find_if(m_GameObjects.begin(), m_GameObjects.end(), [&](CGameObject* pObj) 
	{
		return pObj->Get_ObjectTag() == strObjectTag;
	});

	if (iter == m_GameObjects.end())
		return nullptr;

	return *iter;
}

CGameObject* CLayer::Find_GameObject(_int iObjectID)
{
	READ_LOCK
	CGameObject* pObj = nullptr;
	auto iter = find_if(m_GameObjects.begin(), m_GameObjects.end(), [&](CGameObject* pObj)
		{
			return pObj->Get_ObjectID() == iObjectID;
		});

	if (iter == m_GameObjects.end())
		return nullptr;

	return *iter;
}


CLayer * CLayer::Create()
{
	CLayer*	pInstance = new CLayer();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}

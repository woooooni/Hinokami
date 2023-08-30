#include "..\Public\Object_Manager.h"
#include "GameObject.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

CComponent * CObject_Manager::Get_ComponentPtr(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iLayerIndex)
{
	CLayer*		pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_ComponentPtr(strComponentTag, iLayerIndex);
}

HRESULT CObject_Manager::Reserve_Container(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return S_OK;

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

CGameObject * CObject_Manager::Clone_GameObject(const wstring& strPrototypeTag, void * pArg)
{
	CGameObject*	pPrototype = Find_Prototype(strPrototypeTag);

	if (nullptr == pPrototype)
		return nullptr;

	CGameObject*	pGameObject = pPrototype->Clone(pArg);

	if (nullptr == pGameObject)
		return nullptr;


	return pGameObject;
	
}

// Add_Prototype(TEXT("Prototype_GameObject_Player"), CPlayer::Create());

HRESULT CObject_Manager::Add_Prototype(const wstring& strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(strPrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(strPrototypeTag, pPrototype);			

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObjectToLayer(const wstring& strPrototypeTag, _uint iLevelIndex, const wstring& strLayerTag, void* pArg)
{
	CGameObject*		pGameObject = Clone_GameObject(strPrototypeTag, pArg);

	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer*		pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);

	}
	else
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

void CObject_Manager::Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
			{
				Pair.second->Tick(fTimeDelta);
			}
		}			
	}
	
	
}

void CObject_Manager::LateTick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->LateTick(fTimeDelta);
		}
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
	{
		Safe_Release(Pair.second);
	}

	m_pLayers[iLevelIndex].clear();
}

CGameObject * CObject_Manager::Find_Prototype(const wstring& strPrototypeTag)
{
	auto	iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTag_Finder(strPrototypeTag));

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;	
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	auto	iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTag_Finder(strLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;	
}

void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);


	for (auto& Pair : m_Prototypes)	
		Safe_Release(Pair.second);

	m_Prototypes.clear();
	
}

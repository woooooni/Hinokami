#include "..\Public\Object_Manager.h"
#include "Layer.h"
#include "GameInstance.h"
#include "GameObject.h"


IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Reserve_Manager(_uint iNumLevels, _uint iNumLayerTypes)
{
	if (nullptr != m_pLayers)
		return E_FAIL;


	m_iNumLayer = iNumLayerTypes;
	m_iNumLevels = iNumLevels;

	
	
	m_pPrototypes = new map<const wstring, class CGameObject*>[iNumLayerTypes];


	m_pLayers = new vector<CLayer*>[iNumLevels];

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		m_pLayers[i].reserve(m_iNumLevels + 1);
		for (_uint j = 0; j < iNumLayerTypes; ++j)
			m_pLayers[i].push_back(CLayer::Create());
	}


	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype, _uint iLayerType)
{
	if (nullptr != Find_Prototype(strPrototypeTag, iLayerType))
		return E_FAIL;

	pPrototype->Set_PrototypeTag(strPrototypeTag);
	m_pPrototypes[iLayerType].emplace(strPrototypeTag, pPrototype);

	return S_OK;
}


HRESULT CObject_Manager::Add_GameObject(_uint iLevelIndex, const _uint iLayerType, const wstring & strPrototypeTag, void * pArg, __out class CGameObject** ppOut)
{
	/* 복제할 사본을 차즌ㄷ나. */
	CGameObject*		pPrototype = Find_Prototype(strPrototypeTag, iLayerType);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject*		pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer*		pLayer = Find_Layer(iLevelIndex, iLayerType);
	if (nullptr == pLayer)
		return E_FAIL;
	
	pLayer->Add_GameObject(pGameObject);

	if (ppOut != nullptr)
		*ppOut = pGameObject;
	
	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iLevelIndex, const _uint iLayerType, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, iLayerType);
	if (nullptr == pLayer)
		return E_FAIL;

	pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

CGameObject* CObject_Manager::Clone_GameObject(const wstring& strPrototypeTag, _uint iLayerType, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag, iLayerType);

	if (nullptr == pPrototype)
		return nullptr;

	CGameObject* pGameObject = pPrototype->Clone(pArg);

	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject;
}

const map<const wstring, class CGameObject*>& CObject_Manager::Find_Prototype_GameObjects(_uint iLayerType)
{
	if (iLayerType >= m_iNumLayer)
	{
		MSG_BOX("FAILED : CObject_Manager::Find_Prototype_GameObjects");
		return m_pPrototypes[0];
	}
		
	return  m_pPrototypes[iLayerType];
}

CGameObject* CObject_Manager::Find_Prototype_GameObject(_uint iLayerType, const wstring& strPrototypeTag)
{
	return Find_Prototype(strPrototypeTag, iLayerType);
}

CGameObject* CObject_Manager::Find_GameObject(_uint iLevelIndex, const _uint iLayerType, _int iObjectID)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, iLayerType);
	if (nullptr == pLayer)
		return nullptr;

	CGameObject* pObj = pLayer->Find_GameObject(iObjectID);

	if (nullptr == pObj)
		return nullptr;

	return pObj;
}

CGameObject* CObject_Manager::Find_GameObject(_uint iLevelIndex, const _uint iLayerType, const wstring& strObjectTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, iLayerType);
	if (nullptr == pLayer)
		return nullptr;
	
	CGameObject* pObj = pLayer->Find_GameObject(strObjectTag);

	if (nullptr == pObj)
		return nullptr;

	return pObj;
}

list<CGameObject*>& CObject_Manager::Find_GameObjects(_uint iLevelIndex, const _uint iLayerType)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, iLayerType);
	if (nullptr == pLayer)
	{
		MSG_BOX("Find_GameObejcts Failed.");
		assert(nullptr);
	}

	return m_pLayers[iLevelIndex][iLayerType]->Find_GameObjects();
}

HRESULT CObject_Manager::Clear_Layer(_uint iLevelIndex, const _uint iLayerType)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, iLayerType);
	if (nullptr == pLayer)
	{
		MSG_BOX("Clear_Layer Failed.");
		return E_FAIL;
	}

	pLayer->Free();

	return S_OK;
}

void CObject_Manager::Priority_Tick(_float fTimeDelta)
{

	for (auto& pLayer : m_pLayers[GI->Get_CurrentLevel()])
	{
		pLayer->Priority_Tick(fTimeDelta);
	}
}

void CObject_Manager::Tick(_float fTimeDelta)
{
	_uint iCurrentLevel = GI->Get_CurrentLevel();
	for (auto& pLayer : m_pLayers[GI->Get_CurrentLevel()])
	{
		pLayer->Tick(fTimeDelta);
	}
}

void CObject_Manager::LateTick(_float fTimeDelta)
{
	for (auto& pLayer : m_pLayers[GI->Get_CurrentLevel()])
	{
		pLayer->LateTick(fTimeDelta);
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	for (auto& pLayer : m_pLayers[iLevelIndex])
	{
		pLayer->Clear();
	}
}

CGameObject * CObject_Manager::Find_Prototype(const wstring & strPrototypeTag, _uint iLayerType)
{
	if (iLayerType >= m_iNumLayer)
		return nullptr;

	auto	iter = m_pPrototypes[iLayerType].find(strPrototypeTag);

	if (iter == m_pPrototypes[iLayerType].end())
		return nullptr;

	return iter->second;
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, _uint iLayerType)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer* pLayer = m_pLayers[iLevelIndex][iLayerType];

	if (nullptr == pLayer)
		return nullptr;

	return pLayer;
}

void CObject_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (size_t j = 0; j < m_pLayers[i].size(); ++j)
			Safe_Release(m_pLayers[i][j]);

		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);


	for (_uint i = 0; i < m_iNumLayer; ++i)
	{
		for (auto& pObj : m_pPrototypes[i])
			Safe_Release(pObj.second);
		m_pPrototypes[i].clear();
	}

}

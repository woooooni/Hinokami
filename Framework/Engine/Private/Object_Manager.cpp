#include "..\Public\Object_Manager.h"
#include "Layer.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(strPrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag, void * pArg)
{
	/* 복제할 사본을 차즌ㄷ나. */
	CGameObject*		pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	/* 원형을 보ㅓㄱ제하여 사본을 마느단다. */
	CGameObject*		pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer*		pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* 내가 추가할려고 하는 레이어가 없어ㅓㅆ다. */
	if (nullptr == pLayer)
	{
		/* 레이어를 만드라자. */
		pLayer = CLayer::Create();

		/* 레[이어에 객체를 추가하자. */
		pLayer->Add_GameObject(pGameObject);

		/* 생성한 레이어를 iLevelIndex용 레이어로 추가하였다. */
		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	else /* 추가하고자하는 레이어가 있었다. */
		pLayer->Add_GameObject(pGameObject);
	
	return S_OK;
}

CGameObject* CObject_Manager::Clone_GameObject(const wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);

	if (nullptr == pPrototype)
		return nullptr;

	CGameObject* pGameObject = pPrototype->Clone(pArg);

	if (nullptr == pGameObject)
		return nullptr;


	return pGameObject;
}

void CObject_Manager::Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Tick(fTimeDelta);		
		}
	}

}

void CObject_Manager::LateTick(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
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

CGameObject * CObject_Manager::Find_Prototype(const wstring & strPrototypeTag)
{
	auto	iter = m_Prototypes.find(strPrototypeTag);

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const wstring & strLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Safe_Release(Pair.second);
		}
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();

	
}

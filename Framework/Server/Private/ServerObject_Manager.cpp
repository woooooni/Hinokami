#include "stdafx.h"
#include "ServerObject_Manager.h"
#include "ServerObject.h"

USING(Server)
IMPLEMENT_SINGLETON(CServerObject_Manager)

CServerObject_Manager::CServerObject_Manager()
{

}

HRESULT CServerObject_Manager::Reserve_Manager()
{
	m_pLevelLayerObjects = new map<uint32, vector<class CServerObject*>>[LEVELID::LEVEL_END];

	for (uint32 i = 0; i < LEVELID::LEVEL_END; ++i)
	{
		vector<class CServerObject*> Vector;
		Vector.reserve(100);
		m_pLevelLayerObjects->emplace(i, Vector);
	}


	return S_OK;
}

void CServerObject_Manager::Tick(_float fTimeDelta)
{
}

HRESULT CServerObject_Manager::Add_GameObject(Protocol::S_CREATE_OBJECT& pkt)
{
	WRITE_LOCK
	auto iter = m_pLevelLayerObjects[pkt.ilevelindex()].find(pkt.ilayerindex());
	if (iter == m_pLevelLayerObjects[pkt.ilevelindex()].end())
		return E_FAIL;

	CServerObject* pObj = CServerObject::Create(pkt);
	iter->second.push_back(pObj);

	return S_OK;
}

void CServerObject_Manager::Free()
{
	__super::Free();
}

#include "stdafx.h"
#include "ServerObject.h"


USING(Server)


CServerObject::CServerObject()
{
	


}

CServerObject::~CServerObject()
{

}

HRESULT CServerObject::Initialize(Protocol::S_CREATE_OBJECT& pkt)
{
	m_tObjectInfo.iObjectID = pkt.iobjectid();
	m_tObjectInfo.iObjectType = pkt.iobjecttype();
	m_tObjectInfo.strName = pkt.strname();

	m_tObjectInfo.strPrototypeName = pkt.strprototypename();
	m_tObjectInfo.iModelType = pkt.imodeltype();
	m_tObjectInfo.iAnimationIndex = pkt.ianimationindex();

	memcpy(&m_tObjectInfo.mWorldMatrix, pkt.mutable_mworldmatrix()->mutable_data(), sizeof(_float4x4));

	return S_OK;
}

CServerObject* CServerObject::Create(Protocol::S_CREATE_OBJECT& pkt)
{
	CServerObject* pInstance = new CServerObject;
	if (FAILED(pInstance->Initialize(pkt)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CServerObject::Free()
{
	__super::Free();
}

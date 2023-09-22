#include "Camera_Manager.h"


IMPLEMENT_SINGLETON(CCamera_Manager);
CCamera_Manager::CCamera_Manager()
{

}

CCamera_Manager::~CCamera_Manager()
{

}

void CCamera_Manager::Tick(_float fTimeDelta)
{
	if (nullptr != m_pMainCamera)
		m_pMainCamera->Tick(fTimeDelta);
	
}

void CCamera_Manager::LateTick(_float fTimeDelta)
{
	for (auto& iter : m_Cameras)
		iter.second->LateTick(fTimeDelta);
}

HRESULT CCamera_Manager::Add_Camera(const wstring& strCameraName, CCamera* pCamera)
{
	if (nullptr != Find_Camera(strCameraName))
		return E_FAIL;


	return S_OK;
}

HRESULT CCamera_Manager::Set_MainCamera(const wstring& strCameraName)
{
	CCamera* pCamera = Find_Camera(strCameraName);
	if (nullptr == pCamera)
		return E_FAIL;

	m_pMainCamera = pCamera;
	return S_OK;
}

CCamera* CCamera_Manager::Find_Camera(const wstring& strCameraName)
{
	auto iter = m_Cameras.find(strCameraName);

	if (iter == m_Cameras.end())
		return nullptr;

	return iter->second;
}

HRESULT CCamera_Manager::Clear()
{
	for (auto& iter : m_Cameras)
		Safe_Release(iter.second);

	m_Cameras.clear();

	return S_OK;
}

void CCamera_Manager::Free()
{
	m_pMainCamera = nullptr;
	Clear();

}

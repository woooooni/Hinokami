#include "..\Public\GameObject.h"
#include "GameInstance.h"
#include "Component.h"


CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice); 
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice), m_pContext(rhs.m_pContext)
{
	Safe_AddRef(m_pDevice); 
	Safe_AddRef(m_pContext);
}

CComponent * CGameObject::Get_ComponentPtr(const wstring& strComponentTag)
{
	CComponent*		pComponent = Find_Component(strComponentTag);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent;	
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void * pArg)
{
	return S_OK;
}

void CGameObject::Tick(_float fTimeDelta)
{
}

void CGameObject::LateTick(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, CComponent** ppOut, void* pArg)
{
	if (nullptr != Find_Component(strComponentTag))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	
	CComponent*			pComponent = pGameInstance->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	Safe_Release(pGameInstance);	

	return S_OK;
}

HRESULT CGameObject::Compute_CamZ(_fvector vWorldPos)
{
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	_fvector		vCamPos = XMLoadFloat4(&pPipeLine->Get_CamPosition());

	m_fCamDistance = XMVectorGetX(XMVector3Length(vWorldPos - vCamPos));

	RELEASE_INSTANCE(CPipeLine);

	return S_OK;

}


CComponent * CGameObject::Find_Component(const wstring& strComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(strComponentTag));

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

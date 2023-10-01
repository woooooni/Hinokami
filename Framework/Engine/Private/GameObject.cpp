#include "..\Public\GameObject.h"
#include "GameInstance.h"
#include "Component.h"

CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, wstring strObjectTag)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_strObjectTag(strObjectTag)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_strObjectTag(rhs.m_strObjectTag)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
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

CComponent* CGameObject::Get_Component(const wstring& strComponentTag)
{
	auto iter = m_Components.find(strComponentTag);

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;

}

HRESULT CGameObject::Add_Component(const wstring& strComponentTag, CComponent* pComponent)
{
	if (nullptr != Find_Component(strComponentTag))
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);
	Safe_AddRef(pComponent);
	return S_OK;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const wstring& pPrototypeTag, const wstring& pComponentTag, CComponent** ppOut, void* pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	


	CComponent* pComponent = GAME_INSTANCE->Clone_Component(iLevelIndex, pPrototypeTag, this, pArg);
	if (nullptr == pComponent)
		return E_FAIL;


	pComponent->Set_Owner(this);
	m_Components.emplace(pComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	return S_OK;
}

CComponent* CGameObject::Find_Component(const wstring& strComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(strComponentTag.c_str()));

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

HRESULT CGameObject::Compute_CamZ(_fvector vWorldPos)
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_fvector		vCamPos = XMLoadFloat4(&pPipeLine->Get_CamPosition());

	m_fCamDistance = XMVectorGetX(XMVector3Length(vWorldPos - vCamPos));

	Safe_Release(pPipeLine);

	return S_OK;

}


void CGameObject::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

#include "..\Public\GameObject.h"
#include "GameInstance.h"
#include "Component.h"

_uint CGameObject::g_ObjecId = 0;

CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strObjectTag, _int iObjectType)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_strObjectTag(strObjectTag)
	, m_iObjectType(iObjectType)
	, m_iObjectID(g_ObjecId++)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_strPrototypeTag(rhs.m_strPrototypeTag)
	, m_strObjectTag(rhs.m_strObjectTag)
	, m_iObjectID(g_ObjecId++)
	, m_iObjectType(rhs.m_iObjectType)
	, m_bDead(false)
	, m_bReserveDead(false)
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
	vector<CCollider*> ColliderVector;
	ColliderVector.reserve(10);
	for (_uint i = 0; i < CCollider::DETECTION_TYPE::DETECTION_END; ++i)	
		m_Colliders.emplace(i, ColliderVector);


	return S_OK;
}

void CGameObject::Tick(_float fTimeDelta)
{
}

void CGameObject::LateTick(_float fTimeDelta)
{
	LateUpdate_Collider(fTimeDelta);
}

HRESULT CGameObject::Render()
{
	Render_Collider();
	return S_OK;
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

HRESULT CGameObject::Add_Collider(_uint iLevelIndex, _uint eColliderType, _uint eDetectionType, void* pArg)
{
	CComponent* pComponent = nullptr;
	if(eColliderType == CCollider::COLLIDER_TYPE::SPHERE)
		 pComponent = GAME_INSTANCE->Clone_Component(iLevelIndex, L"Prototype_Component_Sphere_Collider", this, pArg);
	else
		pComponent = GAME_INSTANCE->Clone_Component(iLevelIndex, L"Prototype_Component_AABB_Collider", this, pArg);



	if (nullptr == pComponent)
		return E_FAIL;

	auto iter = m_Colliders.find(eDetectionType);
	if (iter == m_Colliders.end())
	{
		Safe_Release(pComponent);
		return E_FAIL;
	}
		
	CCollider* pCollider = dynamic_cast<CCollider*>(pComponent);
	if (nullptr == pCollider)
	{
		Safe_Release(pComponent);
		return E_FAIL;
	}

	iter->second.push_back(pCollider);
	return S_OK;
}

HRESULT CGameObject::Set_ActiveColliders(_uint eDetectionType, _bool bActive)
{
	auto iter = m_Colliders.find(eDetectionType);
	if (iter == m_Colliders.end())
		return E_FAIL;

	for (auto& pCollider : iter->second)	
		pCollider->Set_Active(bActive);

	return S_OK;
}

void CGameObject::LateUpdate_Collider(_float fTimedelta)
{
	for (auto& Collider : m_Colliders)
	{
		for (auto& pCollider : Collider.second)
			pCollider->LateTick_Collider(fTimedelta);
	}
}

void CGameObject::Render_Collider()
{
	for (auto& Collider : m_Colliders)
	{
		for (auto& pCollider : Collider.second)
			pCollider->Render();
	}
}


void CGameObject::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

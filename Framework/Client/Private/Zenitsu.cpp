#include "stdafx.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Sword.h"
#include "Sweath.h"
#include "Zenitsu.h"

USING(Client)


CZenitsu::CZenitsu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
	: CCharacter(pDevice, pContext, strObjectTag)
{
}

CZenitsu::CZenitsu(const CZenitsu& rhs)
	: CCharacter(rhs)
{

}

HRESULT CZenitsu::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CZenitsu::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Sockets()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	return S_OK;
}

void CZenitsu::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_pStateCom->Tick_State(fTimeDelta);
}

void CZenitsu::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CZenitsu::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CZenitsu::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Zenitsu"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_StateMachine */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateCom)))
		return E_FAIL;


	CNavigation::NAVIGATION_DESC NavigationDesc;
	ZeroMemory(&NavigationDesc, sizeof NavigationDesc);

	XMStoreFloat3(&NavigationDesc.vStartWorldPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	NavigationDesc.bInitialize_Index = true;

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CZenitsu::Ready_States()
{
	//list<wstring> strAnimationName;
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0001_V00_C00_BaseNut01_1");
	//m_pStateCom->Add_State(CCharacter::STATE_IDLE, CState_Tanjiro_Idle::Create(m_pDevice, m_pContext, m_pTransformCom, m_pStateCom, m_pModelCom, strAnimationName));




	//strAnimationName.clear();
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0001_V00_C00_BaseRun01_1");
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0001_V00_C00_BaseDashF01_1");
	//m_pStateCom->Add_State(CCharacter::STATE_RUN,
	//	CState_Tanjiro_Run::Create(m_pDevice,
	//		m_pContext,
	//		m_pTransformCom,
	//		m_pStateCom,
	//		m_pModelCom,
	//		strAnimationName));


	//strAnimationName.clear();
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0000_V00_C00_Death");


	//m_pStateCom->Add_State(CCharacter::STATE_DIE,
	//	CState_Tanjiro_Dead::Create(m_pDevice,
	//		m_pContext,
	//		m_pTransformCom,
	//		m_pStateCom,
	//		m_pModelCom,
	//		strAnimationName));

	//strAnimationName.clear();
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0000_V00_C00_Dmg01_B");
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0000_V00_C00_Dmg01_F");
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0000_V00_C00_Dmg01_L");
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0000_V00_C00_Dmg01_R");
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0000_V00_C00_Dmg01_U");
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0000_V00_C00_Dmg01A_B");
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0000_V00_C00_Dmg01A_F");
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0000_V00_C00_Dmg01A_L");
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0000_V00_C00_Dmg01A_R");
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0000_V00_C00_Dmg01A_U");

	//m_pStateCom->Add_State(CCharacter::STATE_DAMAGED,
	//	CState_Tanjiro_Damaged::Create(m_pDevice,
	//		m_pContext,
	//		m_pTransformCom,
	//		m_pStateCom,
	//		m_pModelCom,
	//		strAnimationName));

	//strAnimationName.clear();
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0001_V00_C00_AtkCmbW01");
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0001_V00_C00_AtkCmbW02");
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0001_V00_C00_AtkCmbW03");
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0001_V00_C00_AtkCmbW03D01");
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0001_V00_C00_AtkCmbW03U01");
	//strAnimationName.push_back(L"SK_P0001_V00_C00.ao|A_P0001_V00_C00_AtkCmbW04");

	//m_pStateCom->Add_State(CCharacter::STATE_ATTACK,
	//	CState_Tanjiro_Attack::Create(m_pDevice,
	//		m_pContext,
	//		m_pTransformCom,
	//		m_pStateCom,
	//		m_pModelCom,
	//		strAnimationName));



	// m_pStateCom->Change_State(CCharacter::STATE_IDLE);
	return S_OK;
}

HRESULT CZenitsu::Ready_Sockets()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	m_Sockets.resize(SOCEKT_END);

	m_Sockets[SOCKET_SWORD] = m_pModelCom->Get_HierarchyNode(L"R_HandCommon_1_Lct");
	m_Sockets[SOCKET_RIGHT_HAND] = m_pModelCom->Get_HierarchyNode(L"R_Hand_1");
	m_Sockets[SOCKET_SWEATH] = m_pModelCom->Get_HierarchyNode(L"L_Weapon_1_Lct");

	return S_OK;
}

HRESULT CZenitsu::Ready_Parts()
{
	m_Parts.resize(PARTTYPE::PART_END);

	CSweath::SWEATH_DESC			SweathDesc;
	SweathDesc.pOwner = this;
	SweathDesc.pParentTransform = m_pTransformCom;
	SweathDesc.pSocketBone = m_Sockets[SOCKET_SWEATH];
	XMStoreFloat3(&SweathDesc.vRotationDegree,
		XMVectorSet(XMConvertToRadians(-180.f),
			XMConvertToRadians(0.f),
			XMConvertToRadians(90.f),
			XMConvertToRadians(0.f)));
	XMStoreFloat4x4(&SweathDesc.SocketPivot, m_pModelCom->Get_PivotMatrix());

	CGameObject* pGameObject = GI->Clone_GameObject(TEXT("Prototype_GameObject_Sweath_Zenitsu"), LAYER_TYPE::LAYER_CHARACTER, &SweathDesc);
	if (nullptr == pGameObject)
		return E_FAIL;

	Safe_AddRef(pGameObject);
	m_Parts[PART_SWEATH] = (pGameObject);


	CSword::SWORD_DESC			SwordDesc;

	SwordDesc.pOwner = this;
	SwordDesc.pParentTransform = m_pTransformCom;
	SwordDesc.pSocketBone = m_Sockets[SOCKET_SWEATH];
	XMStoreFloat3(&SwordDesc.vRotationDegree,
		XMVectorSet(XMConvertToRadians(-180.f),
			XMConvertToRadians(0.f),
			XMConvertToRadians(90.f),
			XMConvertToRadians(0.f)));

	XMStoreFloat4x4(&SwordDesc.SocketPivot, m_pModelCom->Get_PivotMatrix());


	pGameObject = GI->Clone_GameObject(TEXT("Prototype_GameObject_Sword_Zenitsu"), LAYER_TYPE::LAYER_CHARACTER, &SwordDesc);

	if (nullptr == pGameObject)
		return E_FAIL;

	Safe_AddRef(pGameObject);
	m_Parts[PART_SWORD] = pGameObject;


	return S_OK;
}


CZenitsu* CZenitsu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
{
	CZenitsu* pInstance = new CZenitsu(pDevice, pContext, strObjectTag);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CZenitsu");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CZenitsu::Clone(void* pArg)
{
	CZenitsu* pInstance = new CZenitsu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CZenitsu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CZenitsu::Free()
{
	__super::Free();
}


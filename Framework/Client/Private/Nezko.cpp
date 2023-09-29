#include "stdafx.h"
#include "Nezko.h"
#include "GameInstance.h"
#include "HierarchyNode.h"

USING(Client)


CNezko::CNezko(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
	: CCharacter(pDevice, pContext, strObjectTag)
{
}

CNezko::CNezko(const CNezko& rhs)
	: CCharacter(rhs)
{
}

HRESULT CNezko::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CNezko::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/*if (FAILED(Ready_Sockets()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;*/

	if (FAILED(Ready_States()))
		return E_FAIL;

	return S_OK;
}

void CNezko::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CNezko::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CNezko::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNezko::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Nezko"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateCom)))
		return E_FAIL;

	;

	_vector vScale;
	vScale.m128_f32[0] = 0.1f;
	vScale.m128_f32[1] = 0.1f;
	vScale.m128_f32[2] = 0.1f;

	m_pTransformCom->Set_Scale(vScale);

	return S_OK;
}

HRESULT CNezko::Ready_States()
{
	return S_OK;
}

HRESULT CNezko::Ready_Sockets()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CHierarchyNode* pWeaponSocket = m_pModelCom->Get_HierarchyNode(L"SWORD");
	if (nullptr == pWeaponSocket)
		return E_FAIL;

	m_Sockets.push_back(pWeaponSocket);

	return S_OK;
}

HRESULT CNezko::Ready_Parts()
{
	


	/* For.Sword */
	CGameObject* pGameObject = GAME_INSTANCE->Clone_GameObject(TEXT("Prototype_GameObject_Sword"), LAYER_TYPE::LAYER_PLAYER);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_Parts.push_back(pGameObject);

	;

	return S_OK;
}

HRESULT CNezko::Update_Weapon()
{
	if (nullptr == m_Sockets[PART_WEAPON])
		return E_FAIL;

	/* ���. */
	/*_matrix			WeaponMatrix = ���� �����̽� ��ȯ(OffsetMatrix)
		* ���� ���(CombinedTransformation)
		* ���� PivotMatrix * �����̾��ǿ������. ;*/

	_matrix WeaponMatrix = m_Sockets[PART_WEAPON]->Get_OffSetMatrix()
		* m_Sockets[PART_WEAPON]->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		* m_pTransformCom->Get_WorldMatrix();

	m_Parts[PART_WEAPON]->SetUp_State(WeaponMatrix);

	return S_OK;
}

CNezko* CNezko::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
{
	CNezko* pInstance = new CNezko(pDevice, pContext, strObjectTag);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CNezko");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CNezko::Clone(void* pArg)
{
	CNezko* pInstance = new CNezko(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNezko");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNezko::Free()
{
	__super::Free();
}

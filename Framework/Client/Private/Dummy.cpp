#include "stdafx.h"
#include "Dummy.h"
#include "GameInstance.h"


CDummy::CDummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
	:CGameObject(pDevice, pContext, strObjectTag)
{
}

CDummy::CDummy(const CDummy& rhs)
	: CGameObject(rhs)
{

}

HRESULT CDummy::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CDummy::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummy::Ready_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_AnimShader"), (CComponent**)&m_pAnimShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_NonAnim_Shader"), (CComponent**)&m_pNonAnimShaderCom)))
		return E_FAIL;


	Safe_Release(pGameInstance);
	return S_OK;
}


void CDummy::Tick(_float fTimeDelta)
{
	Input(fTimeDelta);
}

void CDummy::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	if (nullptr == m_pModelCom)
		return;

	if(m_pModelCom->Get_ModelType() == CModel::TYPE::TYPE_ANIM)
		m_pModelCom->Play_Animation(fTimeDelta);
	
}

HRESULT CDummy::Render()
{
	if (nullptr == m_pModelCom)
		return S_OK;



	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CShader* pShader = m_pModelCom->Get_ModelType() == CModel::TYPE::TYPE_NONANIM ? m_pNonAnimShaderCom : m_pAnimShaderCom;
	

	if (FAILED(pShader->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(pShader, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(pShader, i)))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CDummy::Ready_ModelCom(_uint eType, const wstring& strFilePath, const wstring& strFileName)
{
	if (nullptr != m_pModelCom)
		Safe_Release(m_pModelCom);
	
	_matrix		PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));


	

	m_pModelCom = CGameInstance::GetInstance()->Import_Model_Data(eType, strFilePath, strFileName, PivotMatrix);

	if (nullptr == m_pModelCom)
		return E_FAIL;


	m_pModelCom->Set_Owner(this);

	return S_OK;
}


HRESULT CDummy::Export_Model_Bin(const wstring& strFilePath, const wstring& strFileName)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	pInstance->Export_Model_Data(m_pModelCom, strFilePath, strFileName);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CDummy::Import_Model()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	return S_OK;
}

void CDummy::Input(_float fTimeDelta)
{
	if (KEY_HOLD(KEY::DOWN_ARROW))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (KEY_HOLD(KEY::LEFT_ARROW))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	}

	if (KEY_HOLD(KEY::RIGHT_ARROW))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	}
	if (KEY_HOLD(KEY::UP_ARROW))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
}




CDummy* CDummy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
{
	CDummy* pInstance = new CDummy(pDevice, pContext, strObjectTag);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CDummy");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CDummy::Clone(void* pArg)
{
	CDummy* pInstance = new CDummy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy::Free()
{
	__super::Free();
}



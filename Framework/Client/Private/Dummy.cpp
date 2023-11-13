#include "stdafx.h"
#include "Dummy.h"
#include "GameInstance.h"


CDummy::CDummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
	:CGameObject(pDevice, pContext, strObjectTag, OBJ_TYPE::OBJ_DUMMY)
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


	;
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
		m_pModelCom->Play_Animation(m_pTransformCom, fTimeDelta);
	
}

HRESULT CDummy::Render()
{
	if (nullptr == m_pModelCom)
		return S_OK;

	CShader* pShader = m_pModelCom->Get_ModelType() == CModel::TYPE::TYPE_NONANIM ? m_pNonAnimShaderCom : m_pAnimShaderCom;
	

	if (FAILED(pShader->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TransPose(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_ViewMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(pShader, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(pShader, i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CDummy::Ready_ModelCom(_uint eType, const wstring& strFilePath, const wstring& strFileName)
{
	if (nullptr != m_pModelCom)
		Safe_Release(m_pModelCom);	

	_tchar szFileName[MAX_PATH];
	_tchar szExt[MAX_PATH];
	_wsplitpath_s(strFileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

	if (FAILED(GAME_INSTANCE->Check_Prototype(LEVEL_DUMMY, wstring(L"Prototype_Componenet_Model_") + szFileName)))
	{
		auto& iter = m_Components.find(wstring(L"Com_Model_") + szFileName);
		if (iter != m_Components.end())
		{
			Safe_Release(iter->second);
			m_Components.erase(iter);
		}

		if (FAILED(__super::Add_Component(LEVEL_DUMMY, wstring(L"Prototype_Componenet_Model_") + szFileName, wstring(L"Com_Model_") + szFileName, (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(GAME_INSTANCE->Import_Model_Data(LEVEL_DUMMY, wstring(L"Prototype_Componenet_Model_") + szFileName, eType, strFilePath, strFileName, &m_pModelCom)))
			return E_FAIL;
	}
	m_pModelCom->Set_Owner(this);

	return S_OK;
}


HRESULT CDummy::Export_Model_Bin(const wstring& strFilePath, const wstring& strFileName)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	
	if (FAILED(GAME_INSTANCE->Export_Model_Data(m_pModelCom, strFilePath, strFileName)))
		return E_FAIL;

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



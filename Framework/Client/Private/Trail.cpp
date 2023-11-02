#include "stdafx.h"
#include "GameInstance.h"
#include "Trail.h"


USING(Client)
CTrail::CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
	: CGameObject(pDevice, pContext, strObjectTag, OBJ_TYPE::OBJ_TRAIL)
{
}

CTrail::CTrail(const CTrail& rhs)
	: CGameObject(rhs)
{	

}


HRESULT CTrail::Initialize_Prototype(const CVIBuffer_Trail::TRAIL_DESC& TrailDesc)
{
	m_TrailDesc = TrailDesc;

	return S_OK;
}

HRESULT CTrail::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CTrail::Tick(_float fTimeDelta)
{
	m_pVIBufferCom->Update_Trail(fTimeDelta, XMLoadFloat4x4(&m_TransformMatrix));
}

void CTrail::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CTrail::Render()
{
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMMatrixIdentity()));

	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (m_pShaderCom->Bind_RawValue("g_TrailColor", &m_TrailDesc.vColor, sizeof(_float4)))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();
	return S_OK;
}

HRESULT CTrail::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &m_TrailDesc)))
		return E_FAIL;



	return S_OK;
}

void CTrail::SetUp_Position(_vector vHighPosition, _vector vLowPosition)
{
	m_pVIBufferCom->SetUp_Position(vHighPosition, vLowPosition);
}

void CTrail::Start_Trail(_matrix TransformMatrix)
{
	m_pVIBufferCom->Start_Trail(TransformMatrix);
}

void CTrail::Stop_Trail()
{
	m_pVIBufferCom->Stop_Trail();
}


CTrail* CTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const CVIBuffer_Trail::TRAIL_DESC& TrailDesc)
{
	CTrail* pInstance = new CTrail(pDevice, pContext, strObjectTag);
	if (FAILED(pInstance->Initialize_Prototype(TrailDesc)))
	{
		MSG_BOX("Create Failed : CTrail");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CTrail::Clone(void* pArg)
{
	CTrail* pInstance = new CTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNezko");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail::Free()
{
	__super::Free();

}

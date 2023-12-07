#include "stdafx.h"
#include "UI_GaugeBar.h"
#include "GameInstance.h"
#include "Npc.h"
#include "Monster.h"
#include "Character.h"

CUI_GaugeBar::CUI_GaugeBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext, L"UI_GaugeBar")
{

}

CUI_GaugeBar::CUI_GaugeBar(const CUI_GaugeBar & rhs)
	: CUI(rhs)
	, m_eGaugeType(rhs.m_eGaugeType)
{
	
}

HRESULT CUI_GaugeBar::Initialize_Prototype(GAUGE_TYPE eGaugeType, GAUGE_BAR_POSITION ePosition)
{
	m_eGaugeType = eGaugeType;
	m_eBarPositon = ePosition;
	switch (ePosition)
	{
	case LEFT_TOP:
		m_tInfo.fX = g_iWinSizeX / 4.f;
		m_tInfo.fY = 90.f;
		m_tInfo.fCX = 500.f;
		m_tInfo.fCY = 15.f;
		break;
	case RIGHT_TOP: 
		m_tInfo.fX = g_iWinSizeX - (g_iWinSizeX / 6.f);
		m_tInfo.fY = 90.f;
		m_tInfo.fCX = 500.f;
		m_tInfo.fCY = 15.f;
		break;

	case LEFT_TOP_BOTTOM:
		m_tInfo.fX = g_iWinSizeX / 4.f;
		m_tInfo.fY = 120.f;
		m_tInfo.fCX = 500.f;
		m_tInfo.fCY = 15.f;
		break;

	case RIGHT_TOP_BOTTOM:
		m_tInfo.fX = g_iWinSizeX - (g_iWinSizeX / 6.f);
		m_tInfo.fY = 120.f;
		m_tInfo.fCX = 500.f;
		m_tInfo.fCY = 15.f;
		break;
	
	}
	return S_OK;
}

HRESULT CUI_GaugeBar::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_pTransformCom->Set_Scale(XMLoadFloat3(&_float3(m_tInfo.fCX, m_tInfo.fCY, 1.f)));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_tInfo.fX - g_iWinSizeX * 0.5f, -m_tInfo.fY + g_iWinSizeY * 0.5f, 0.02f, 1.f));


	m_pBackGroundTransform->Set_Scale(XMLoadFloat3(&_float3(m_tInfo.fCX + 10.f, m_tInfo.fCY + 10.f, 1.f)));
	m_pBackGroundTransform->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_tInfo.fX - g_iWinSizeX * 0.5f, -m_tInfo.fY + g_iWinSizeY * 0.5f, 0.03f, 1.f));


	_float3 vIconScale = _float3(360.f, 120.f, 1.f);
	m_pIconTransformCom->Set_Scale(XMLoadFloat3(&vIconScale));
	m_pIconTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet((m_tInfo.fX - 200.f) - g_iWinSizeX * 0.5f, -1.f * (m_tInfo.fY) + g_iWinSizeY * 0.5f, 0.02f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.01f, 1.f));

	// ShowCursor(false);

	return S_OK;
}

void CUI_GaugeBar::Debug_Input(_float fTimeDelta)
{

}


void CUI_GaugeBar::Tick(_float fTimeDelta)
{
	if (nullptr != m_pOwnerCharacter)
	{
		if (m_pOwnerCharacter->Is_Dead())
		{
			Safe_Release(m_pOwnerCharacter);
			m_pOwnerCharacter = nullptr;
			return;
		}
		const CCharacter::CHARACTER_STAT& CharacterStatDesc = m_pOwnerCharacter->Get_Stat();
		m_fDestRatio = max(0.f, fabs(CharacterStatDesc.fHp / CharacterStatDesc.fMaxHp));
	}
	else if (nullptr != m_pOwnerMonster)
	{
		if (m_pOwnerMonster->Is_Dead())
		{
			Safe_Release(m_pOwnerMonster);
			m_pOwnerMonster = nullptr;
			return;
		}

		const CMonster::MONSTER_STAT& MonsterStatDesc = m_pOwnerMonster->Get_Stat();
		m_fDestRatio = max(0.f, fabs(MonsterStatDesc.fHp / MonsterStatDesc.fMaxHp));
	}


	
	m_pTransformCom->Set_Scale(XMLoadFloat3(&_float3(m_tInfo.fCX * m_fDestRatio, m_tInfo.fCY, 1.f)));

	_float fMovePosX = m_tInfo.fCX - (m_tInfo.fCX * m_fDestRatio);

	if (m_eBarPositon == GAUGE_BAR_POSITION::LEFT_TOP || m_eBarPositon == GAUGE_BAR_POSITION::LEFT_TOP_BOTTOM)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION,
			XMVectorSet((m_tInfo.fX - (fMovePosX / 2.f)) - g_iWinSizeX * 0.5f, -m_tInfo.fY + g_iWinSizeY * 0.5f, 0.02f, 1.f));
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION,
			XMVectorSet((m_tInfo.fX + (fMovePosX / 2.f)) - g_iWinSizeX * 0.5f, -m_tInfo.fY + g_iWinSizeY * 0.5f, 0.02f, 1.f));
	}
	
}

void CUI_GaugeBar::LateTick(_float fTimeDelta)
{
	if(m_pOwnerCharacter || m_pOwnerMonster || m_pOwnerNpc)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RENDER_UI, this);
}

HRESULT CUI_GaugeBar::Render()
{
	// BackGround
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pBackGroundTransform->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pBackGroundTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();



	// Gauge
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_eGaugeType)))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

	if (m_eBarPositon == GAUGE_BAR_POSITION::LEFT_TOP_BOTTOM || m_eBarPositon == GAUGE_BAR_POSITION::RIGHT_TOP_BOTTOM)
		return S_OK;

	// Icon
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pIconTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pIconTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_eCharacterType)))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();



	return S_OK;
}

HRESULT CUI_GaugeBar::Ready_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GaugeBar"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_BackGroundTexture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gauge_BackGround"),
		TEXT("Com_BackGroundTexture"), (CComponent**)&m_pBackGroundTextureCom)))
		return E_FAIL;

	/* Com_IconTexture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Character_Icon"),
		TEXT("Com_IconTexture"), (CComponent**)&m_pIconTextureCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_BackGround"), (CComponent**)&m_pBackGroundTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Icon"), (CComponent**)&m_pIconTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}



HRESULT CUI_GaugeBar::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	return S_OK;
}

void CUI_GaugeBar::Reset()
{
	m_fCurrRatio = 0.f;
	m_fDestRatio = 1.f;

	Safe_Release(m_pOwnerCharacter);
	Safe_Release(m_pOwnerMonster);
	Safe_Release(m_pOwnerNpc);

	m_pOwnerCharacter = nullptr;
	m_pOwnerMonster = nullptr;
	m_pOwnerNpc = nullptr;
}

CUI_GaugeBar * CUI_GaugeBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, GAUGE_TYPE eGaugeType, GAUGE_BAR_POSITION ePosition)
{
	CUI_GaugeBar*	pInstance = new CUI_GaugeBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eGaugeType, ePosition)))
	{
		MSG_BOX("Failed to Created : CUI_GaugeBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CUI_GaugeBar::Set_Owner(class CNpc* pOwner, CHARACTER_TYPE eType) {
	Reset();
	m_pOwnerNpc = pOwner;
	Safe_AddRef(pOwner);
	m_eCharacterType = eType;
}
void CUI_GaugeBar::Set_Owner(class CMonster* pOwner, CHARACTER_TYPE eType) {
	Reset();
	m_pOwnerMonster = pOwner;
	Safe_AddRef(pOwner);
	m_eCharacterType = eType;
}
void CUI_GaugeBar::Set_Owner(class CCharacter* pOwner, CHARACTER_TYPE eType) {
	Reset();
	m_pOwnerCharacter = pOwner;
	Safe_AddRef(pOwner);
	m_eCharacterType = eType;
}

CGameObject * CUI_GaugeBar::Clone(void* pArg)
{
	CUI_GaugeBar*	pInstance = new CUI_GaugeBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_GaugeBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_GaugeBar::Free()
{
	__super::Free();

	Safe_Release(m_pBackGroundTransform);
	Safe_Release(m_pBackGroundTextureCom);
}

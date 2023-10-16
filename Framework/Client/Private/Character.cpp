#include "stdafx.h"
#include "GameInstance.h"
#include "Character.h"
#include "HierarchyNode.h"
#include "Key_Manager.h"
#include "Part.h"


USING(Client)
CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
	: CGameObject(pDevice, pContext, strObjectTag, OBJ_TYPE::OBJ_CHARACTER)
{
	
}

CCharacter::CCharacter(const CCharacter& rhs)
	: CGameObject(rhs)
	, m_pShaderCom(rhs.m_pShaderCom)
	, m_pRendererCom(rhs.m_pRendererCom)
	, m_pTransformCom(rhs.m_pTransformCom)
	, m_pModelCom(rhs.m_pModelCom)
	, m_pStateCom(rhs.m_pStateCom)	
{	
	Safe_AddRef(m_pShaderCom);
	Safe_AddRef(m_pRendererCom);
	Safe_AddRef(m_pTransformCom);
	Safe_AddRef(m_pModelCom);
	Safe_AddRef(m_pStateCom);
}

HRESULT CCharacter::Initialize_Prototype()
{
	if(FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacter::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCharacter::Tick(_float fTimeDelta)
{
	Input(fTimeDelta);

	for (auto& pPart : m_Parts)
		pPart->Tick(fTimeDelta);
}

void CCharacter::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pModelCom->Play_Animation(m_pTransformCom, fTimeDelta);

	for (auto& pPart : m_Parts)
		pPart->LateTick(fTimeDelta);


	for (auto& pPart : m_Parts)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, pPart);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CCharacter::Render()
{
	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}

	return S_OK;
}

CHierarchyNode* CCharacter::Get_Socket(PARTTYPE eType)
{
	if (eType >= PARTTYPE::PART_END)
		return nullptr;


	return m_Sockets[eType];
}

CHierarchyNode* CCharacter::Get_Socket(const wstring& strSocketName)
{
	for (auto& pSocket : m_Sockets)
	{
		if (pSocket->Get_Name() == strSocketName)
			return pSocket;
	}
	return nullptr;
}





void CCharacter::Input(_float fTimeDelta)
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

	if (KEY_TAP(KEY::OPEN_SQUARE_BRACKET))
	{
		m_pModelCom->Set_AnimIndex(m_pModelCom->Get_CurrAnimationIndex() - 1);
	}
	if (KEY_TAP(KEY::CLOSE_SQUARE_BRACKET))
	{
		m_pModelCom->Set_AnimIndex(m_pModelCom->Get_CurrAnimationIndex() + 1);
	}
}


void CCharacter::Free()
{
	__super::Free();

	for (auto& pPart : m_Parts)
		Safe_Release(pPart);

	m_Parts.clear();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

}

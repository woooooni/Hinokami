#include "stdafx.h"
#include "GameInstance.h"
#include "Character.h"
#include "HierarchyNode.h"
#include "Key_Manager.h"
#include "Part.h"
#include "Sword.h"
#include "Sweath.h"
#include <future>
#include "Trail.h"


USING(Client)
CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
	: CGameObject(pDevice, pContext, strObjectTag, OBJ_TYPE::OBJ_CHARACTER)
{
	
}

CCharacter::CCharacter(const CCharacter& rhs)
	: CGameObject(rhs)
{	

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

	for (auto& pPart : m_Parts)
		pPart->Tick(fTimeDelta);

	if (m_bInfinite)
	{
		m_fAccInfinite += fTimeDelta;
		if (m_fAccInfinite >= m_fInfiniteTime)
		{
			m_bInfinite = false;
			m_fAccInfinite = 0.f;

			Set_ActiveColliders(CCollider::DETECTION_TYPE::HEAD, true);
			Set_ActiveColliders(CCollider::DETECTION_TYPE::BODY, true);
		}
	}
	

	for(_uint i = 0; i < SOCEKT_END; ++i)
	{
		if (nullptr == m_pTrails[i])
			continue;
		_matrix		WorldMatrix = m_Sockets[i]->Get_CombinedTransformation() * m_pModelCom->Get_PivotMatrix();
		WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
		WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
		WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

		m_pTrails[i]->Set_TransformMatrix(WorldMatrix * m_pTransformCom->Get_WorldMatrix());
		m_pTrails[i]->Tick(fTimeDelta);
		
	}
}

void CCharacter::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	
	std::async(&CModel::Play_Animation, m_pModelCom, m_pTransformCom, fTimeDelta);
	

	for (auto& pPart : m_Parts)
		pPart->LateTick(fTimeDelta);


	for (auto& pPart : m_Parts)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, pPart);
		//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, pPart);
	}
		

	__super::LateTick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);



	for (_uint i = 0; i < SOCEKT_END; ++i)
	{
		if (nullptr == m_pTrails[i])
			continue;

		m_pTrails[i]->LateTick(fTimeDelta);
	}
		

}

HRESULT CCharacter::Render()
{
	__super::Render();

	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TransPose(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint		iPassIndex = 0;

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;
		else
			iPassIndex++;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	

	return S_OK;
}

HRESULT CCharacter::Render_ShadowDepth()
{
	__super::Render_ShadowDepth();

	if (nullptr == m_pShaderCom ||
		nullptr == m_pTransformCom)
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TransPose(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewMatrix", &GI->Get_ShadowViewMatrix(GI->Get_CurrentLevel()), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GI->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(0), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 10)))
			return E_FAIL;
	}


	return S_OK;
}

void CCharacter::Collision_Enter(const COLLISION_INFO& tInfo)
{
}

void CCharacter::Collision_Continue(const COLLISION_INFO& tInfo)
{

}

void CCharacter::Collision_Exit(const COLLISION_INFO& tInfo)
{
	if (tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_MONSTER
		|| tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_CHARACTER)
	{
		if (tInfo.pOtherCollider->Get_DetectionType() == CCollider::BODY
			&& tInfo.pMyCollider->Get_DetectionType() == CCollider::BODY)
		{
			if (m_pStateCom->Get_CurrState() == CCharacter::STATE::DAMAGED_BLOW)
				return;

			_float3 vVelocity = m_pRigidBodyCom->Get_Velocity();

			vVelocity.x = 0.f;
			vVelocity.z = 0.f;

			m_pRigidBodyCom->Set_Velocity(vVelocity);

		}
	}
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

void CCharacter::DrawSword()
{
	if (nullptr == m_Parts[PARTTYPE::PART_SWORD])
		return;

	CSword* pSword = dynamic_cast<CSword*>(m_Parts[PARTTYPE::PART_SWORD]);
	if (nullptr == pSword)
		return;
	
	if (pSword->Get_Current_SocketBone() == m_Sockets[SOCKET_SWORD])
		return;

	pSword->Set_OriginRotation_Transform(XMMatrixRotationQuaternion(
		XMQuaternionRotationRollPitchYaw(XMConvertToRadians(180.f), XMConvertToRadians(0.f), XMConvertToRadians(-90.f))));

	pSword->Set_SocketBone(m_Sockets[SOCKET_SWORD]);
}

void CCharacter::SweathSword()
{
	if (nullptr == m_Parts[PARTTYPE::PART_SWORD])
		return;

	CSword* pSword = dynamic_cast<CSword*>(m_Parts[PARTTYPE::PART_SWORD]);
	if (nullptr == pSword)
		return;

	if (pSword->Get_Current_SocketBone() == m_Sockets[SOCKET_SWEATH])
		return;

	pSword->Set_OriginRotation_Transform(XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(pSword->Get_PrevRotation().x, pSword->Get_PrevRotation().y, pSword->Get_PrevRotation().z)));
	pSword->Set_SocketBone(m_Sockets[SOCKET_SWEATH]);
}

void CCharacter::Set_Infinite(_float fInfiniteTime, _bool bInfinite)
{
	m_bInfinite = bInfinite;
	m_fInfiniteTime = fInfiniteTime;
	m_fAccInfinite = 0.f;

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

	for (_uint i = 0; i < SOCEKT_END; ++i)
	{
		if (nullptr == m_pTrails[i])
			continue;

		Safe_Release(m_pTrails[i]);
	}

}

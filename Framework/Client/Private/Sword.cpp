#include "stdafx.h"
#include "..\Public\Sword.h"

#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Collision_Manager.h"
#include "Monster.h"
#include "Trail.h"
#include "PipeLine.h"
#include "Particle_Manager.h"

CSword::CSword(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strObjectTag)
	: CPart(pDevice, pContext, strObjectTag, OBJ_TYPE::OBJ_WEAPON)
{

}

CSword::CSword(const CSword & rhs)
	: CPart(rhs)
	, m_strModelPrototype(rhs.m_strModelPrototype)
{

}

HRESULT CSword::Initialize_Prototype(const wstring& strPrototypeSwordModel)
{
	m_strModelPrototype = strPrototypeSwordModel;
	return S_OK;
}

HRESULT CSword::Initialize(void* pArg)
{
	SWORD_DESC* pWeaponDesc = nullptr;
	if (nullptr != pArg)
	{
		pWeaponDesc = (SWORD_DESC*)pArg;

		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
		m_eType = pWeaponDesc->eType;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pWeaponDesc != nullptr)
	{
		m_vPrevRotation = pWeaponDesc->vRotationDegree;

		XMStoreFloat4x4(&m_OriginRotationTransform, XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(pWeaponDesc->vRotationDegree.x), XMConvertToRadians(pWeaponDesc->vRotationDegree.y), XMConvertToRadians(pWeaponDesc->vRotationDegree.z))));
	}
	else
		return E_FAIL;

	if (FAILED(Ready_Colliders()))
		return E_FAIL;

	Stop_Trail();

	return S_OK;
}

void CSword::Tick(_float fTimeDelta)
{
	GI->Add_CollisionGroup(COLLISION_GROUP::CHARACTER, this);

	_float3 vRotation = _float3(0.f, 180.f, 90.f);
	switch (m_eType)
	{
	case SWORD_TYPE::TANJIRO:
		vRotation = m_bSweath ? _float3(-90.f, 90.f, 90.f) : _float3(0.f, 180.f, 90.f);
		break;
	case SWORD_TYPE::ZENITSU:
		vRotation = m_bSweath ? _float3(0.f, 180.f, 270.f) : _float3(0.f, 180.f, 90.f);
		break;
	case SWORD_TYPE::KYOJURO:
		vRotation = _float3(0.f, 180.f, 0.f);
		break;
	}
	
	XMStoreFloat4x4(&m_OriginRotationTransform,
		XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(vRotation.x), XMConvertToRadians(vRotation.y), XMConvertToRadians(vRotation.z))));
	


	_matrix		WorldMatrix = m_pSocketBone->Get_CombinedTransformation() * XMLoadFloat4x4(&m_SocketPivotMatrix);

	WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);
	

	Compute_RenderMatrix(WorldMatrix);


	m_pTrailObject->Set_TransformMatrix(m_pTransformCom->Get_WorldMatrix());
	m_pTrailObject->Tick(fTimeDelta);
}

void CSword::LateTick(_float fTimeDelta)
{
	m_pTrailObject->LateTick(fTimeDelta);
	__super::LateTick(fTimeDelta);
	
}

HRESULT CSword::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint		iPassIndex = 0;
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
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

void CSword::Collision_Enter(const COLLISION_INFO& tInfo)
{

}

void CSword::Collision_Continue(const COLLISION_INFO& tInfo)
{

}

void CSword::Collision_Exit(const COLLISION_INFO& tInfo)
{

}

_matrix CSword::Get_FinalWorldMatrix()
{
	return m_pTransformCom->Get_WorldMatrix();
}


void CSword::SetUp_Trail_Position(_vector vHighPosition, _vector vLowPosition)
{
	m_pTrailObject->SetUp_Position(vHighPosition, vLowPosition);
}

void CSword::Generate_Trail(const wstring& strDiffuseTextureName, const wstring& strAlphaTextureName, const _float4& vColor, _uint iVertexCount)
{
	Compute_RenderMatrix(m_pSocketBone->Get_CombinedTransformation() * XMLoadFloat4x4(&m_SocketPivotMatrix));

	m_pTrailObject->Set_DiffuseTexture_Index(strDiffuseTextureName);
	m_pTrailObject->Set_AlphaTexture_Index(strAlphaTextureName);

	CTrail::TRAIL_DESC TrailDesc = m_pTrailObject->Get_TrailDesc();
	TrailDesc.vDiffuseColor = vColor;
	m_pTrailObject->Set_TrailDesc(TrailDesc);

	m_pTrailObject->Set_VtxCount(iVertexCount);
	m_pTrailObject->Start_Trail(m_pTransformCom->Get_WorldMatrix());
}

void CSword::Stop_Trail()
{
	m_pTrailObject->Stop_Trail();
}


HRESULT CSword::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strModelPrototype,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CTrail::TRAIL_DESC TrailDesc = { };
	TrailDesc.bTrail = false;
	TrailDesc.fAccGenTrail = 0.f;
	TrailDesc.fGenTrailTime = 1.f;
	TrailDesc.vDiffuseColor = { 1.f, 0.f, 0.f, 0.5f };
	TrailDesc.vUV_FlowSpeed = { 0.f, 0.f };
	TrailDesc.bUV_Cut = -1;

	m_pTrailObject = CTrail::Create(m_pDevice, m_pContext, L"Sword_Trail", TrailDesc);
	if (m_pTrailObject == nullptr)
		return E_FAIL;


	if (FAILED(m_pTrailObject->Initialize(nullptr)))
		return E_FAIL;

	m_pTrailObject->SetUp_Position(XMVectorSet(0.f, 0.f, 0.5f, 1.f), XMVectorSet(0.f, 0.f, -0.9f, 1.f));

	return S_OK;
}

HRESULT CSword::Ready_Colliders()
{

	CCollider_Sphere::SPHERE_COLLIDER_DESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	BoundingSphere tSphere;
	ZeroMemory(&tSphere, sizeof(BoundingSphere));
	tSphere.Radius = 1.f;

	XMStoreFloat4x4(&ColliderDesc.ModePivotMatrix, m_pModelCom->Get_PivotMatrix());

	ColliderDesc.tSphere = tSphere;

	ColliderDesc.tSphere.Radius = 1.f;
	ColliderDesc.pOwnerTransform = m_pTransformCom;
	ColliderDesc.vOffsetPosition = _float3(0.f, 0.f, 50.f);
	XMStoreFloat4x4(&ColliderDesc.ModePivotMatrix, m_pModelCom->Get_PivotMatrix());

	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider_Sphere::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::BOUNDARY, &ColliderDesc)))
		return E_FAIL;



	CCollider_OBB::OBB_COLLIDER_DESC OBBDesc;
	ZeroMemory(&OBBDesc, sizeof OBBDesc);
	OBBDesc.tBox.Extents = _float3(5.f, 5.f, 60.f);
	XMStoreFloat4(&OBBDesc.tBox.Orientation, XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f)));


	OBBDesc.pOwnerTransform = m_pTransformCom;
	OBBDesc.vOffsetPosition = _float3(0.f, 0.f, 60.f);
	
	XMStoreFloat4x4(&OBBDesc.ModePivotMatrix, m_pModelCom->Get_PivotMatrix());

	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider_Sphere::COLLIDER_TYPE::OBB, CCollider::DETECTION_TYPE::ATTACK, &OBBDesc)))
		return E_FAIL;
	//ColliderDesc.tSphere.Radius = 0.2f;
	//ColliderDesc.vOffsetPosition = _float3(0.f, 0.f, 130.f);
	//if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider_Sphere::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::ATTACK, &ColliderDesc)))
	//	return E_FAIL;

	//ColliderDesc.tSphere.Radius = 0.2f;
	//ColliderDesc.vOffsetPosition = _float3(0.f, 0.f, 110.f);
	//if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider_Sphere::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::ATTACK, &ColliderDesc)))
	//	return E_FAIL;

	//ColliderDesc.vOffsetPosition = _float3(0.f, 0.f, 90.f);
	//if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider_Sphere::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::ATTACK, &ColliderDesc)))
	//	return E_FAIL;

	//ColliderDesc.vOffsetPosition = _float3(0.f, 0.f, 70.f);
	//if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider_Sphere::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::ATTACK, &ColliderDesc)))
	//	return E_FAIL;

	//ColliderDesc.vOffsetPosition = _float3(0.f, 0.f, 50.f);
	//if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider_Sphere::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::ATTACK, &ColliderDesc)))
	//	return E_FAIL;

	//ColliderDesc.vOffsetPosition = _float3(0.f, 0.f, 30.f);
	//if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider_Sphere::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::ATTACK, &ColliderDesc)))
	//	return E_FAIL;

	vector<CCollider*>& Colliders = Get_Collider(CCollider::DETECTION_TYPE::ATTACK);
	for (auto& pCollider : Colliders)
	{
		pCollider->Set_ColliderID(Colliders[0]->Get_ColliderID());
	}

	return S_OK;
}

HRESULT CSword::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TransPose(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	return S_OK;
}


CSword * CSword::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strObjectTag, const wstring& strPrototypeSwordModel)
{
	CSword*	pInstance = new CSword(pDevice, pContext, strObjectTag);

	if (FAILED(pInstance->Initialize_Prototype(strPrototypeSwordModel)))
	{
		MSG_BOX("Failed to Created : CSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSword::Clone(void* pArg)
{
	CSword*	pInstance = new CSword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSword::Free()
{
	__super::Free();
	Safe_Release(m_pTrailObject);
}
;
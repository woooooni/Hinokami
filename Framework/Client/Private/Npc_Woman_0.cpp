#include "stdafx.h"
#include "GameInstance.h"
#include "Npc_Woman_0.h"
#include "State_Npc_Idle.h"



USING(Client)
CNpc_Woman_0::CNpc_Woman_0(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNpc(pDevice, pContext, L"Npc_Woman_0")
{
	
}

CNpc_Woman_0::CNpc_Woman_0(const CNpc_Woman_0& rhs)
	: CNpc(rhs)	

{	
	
}

HRESULT CNpc_Woman_0::Initialize_Prototype()
{
	if(FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNpc_Woman_0::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	if (FAILED(Ready_Colliders()))
		return E_FAIL;

	return S_OK;
}

void CNpc_Woman_0::Tick(_float fTimeDelta)
{
	if (m_bReserveDead)
	{
		m_fDissolveWeight += 0.2f * fTimeDelta;
		if (m_fDissolveWeight >= 1.f)
			Set_Dead(true);
	}
}

void CNpc_Woman_0::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	std::async(&CModel::Play_Animation, m_pModelCom, m_pTransformCom, fTimeDelta);
		

	__super::LateTick(fTimeDelta);
	if (true == GI->Intersect_Frustum_World(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}
}

HRESULT CNpc_Woman_0::Render()
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

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_NormalTexture")))
			iPassIndex = 0;
		else
			iPassIndex++;

		if (true == m_bReserveDead)
		{
			iPassIndex = 2;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveWeight", &m_fDissolveWeight, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pDissoveTexture->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CNpc_Woman_0::Render_ShadowDepth()
{

	if (nullptr == m_pShaderCom || nullptr == m_pTransformCom)
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TransPose(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &GI->Get_ShadowViewMatrix(GI->Get_CurrentLevel()))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GI->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;



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

void CNpc_Woman_0::Collision_Enter(const COLLISION_INFO& tInfo)
{

}

void CNpc_Woman_0::Collision_Continue(const COLLISION_INFO& tInfo)
{
	if (tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_MONSTER || tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_CHARACTER)
	{
		CTransform* pOtherTransform = tInfo.pOther->Get_Component<CTransform>(L"Com_Transform");
		_vector vTargetDir = XMVector3Normalize(pOtherTransform->Get_State(CTransform::STATE::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));

		_float fTargetLen = XMVectorGetX(XMVector3Length(vTargetDir));
		if (tInfo.pOtherCollider->Get_DetectionType() == CCollider::BODY
			&& tInfo.pMyCollider->Get_DetectionType() == CCollider::BODY)
		{
			if (fTargetLen <= 0.f)
				vTargetDir = XMVectorSetX(vTargetDir, _float(rand() % 10 - 5));

			vTargetDir = XMVectorSetY(vTargetDir, 0.f);
			vTargetDir = XMVector3Normalize(vTargetDir);
			vTargetDir *= -1.f;

			_float fForce = (tInfo.pMyCollider->Get_Radius() + tInfo.pOtherCollider->Get_Radius()) - fTargetLen;

			CRigidBody* pOtherRigidBody = tInfo.pOther->Get_Component<CRigidBody>(L"Com_RigidBody");
			if (pOtherRigidBody)			
				fForce += XMVectorGetX(XMVector3Length(XMLoadFloat3(&pOtherRigidBody->Get_Velocity())));

			m_pRigidBodyCom->Add_Velocity_Acc(vTargetDir, fForce);
		}
	}
}

void CNpc_Woman_0::Collision_Exit(const COLLISION_INFO& tInfo)
{
	
	if (tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_MONSTER
		|| tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_CHARACTER)
	{
		if (tInfo.pOtherCollider->Get_DetectionType() == CCollider::BODY
			&& tInfo.pMyCollider->Get_DetectionType() == CCollider::BODY)
		{

			_float3 vVelocity = m_pRigidBodyCom->Get_Velocity();

			vVelocity.x = 0.f;
			vVelocity.z = 0.f;

			m_pRigidBodyCom->Set_Velocity(vVelocity);
		}
	}
}





void CNpc_Woman_0::On_Damaged(CGameObject* pAttacker, _uint eDamageType, _float fDamage)
{
	__super::On_Damaged(pAttacker, eDamageType, fDamage);
}

HRESULT CNpc_Woman_0::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Npc_Woman_0"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_StateMachine */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateCom)))
		return E_FAIL;

	/* For.Com_DissolveTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissoveTexture)))
		return E_FAIL;

	CNavigation::NAVIGATION_DESC NavigationDesc;
	ZeroMemory(&NavigationDesc, sizeof NavigationDesc);

	XMStoreFloat4(&NavigationDesc.vStartWorldPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	NavigationDesc.bInitialize_Index = true;

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
		return E_FAIL;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat4(&m_pNavigationCom->Get_NaviDesc().vStartWorldPosition), 1.f));

	CRigidBody::RIGID_BODY_DESC RigidDesc;
	ZeroMemory(&RigidDesc, sizeof RigidDesc);


	RigidDesc.pNavigation = m_pNavigationCom;
	RigidDesc.pTransform = m_pTransformCom;

	/* For.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigidBodyCom, &RigidDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNpc_Woman_0::Ready_States()
{
	list<wstring> strAnimationName;
	strAnimationName.push_back(L"SK_N0017_V00_C00.ao|A_N0017_V00_C00_AdvStand01_1");
	if (FAILED(m_pStateCom->Add_State(CNpc::NPC_STATE::IDLE, CState_Npc_Idle::Create(m_pDevice, m_pContext, m_pStateCom, strAnimationName))))
		return E_FAIL;

	m_pStateCom->Change_State(CNpc::NPC_STATE::IDLE);


	return S_OK;
}

HRESULT CNpc_Woman_0::Ready_Colliders()
{
	CCollider_Sphere::SPHERE_COLLIDER_DESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	BoundingSphere tSphere;
	ZeroMemory(&tSphere, sizeof(BoundingSphere));
	tSphere.Radius = 1.f;
	XMStoreFloat4x4(&ColliderDesc.ModePivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColliderDesc.pOwnerTransform = m_pTransformCom;

	ColliderDesc.tSphere = tSphere;
	ColliderDesc.pNode = m_pModelCom->Get_HierarchyNode(L"Root");
	ColliderDesc.vOffsetPosition = _float3(0.f, 1.f, 0.f);

	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::BOUNDARY, &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.tSphere.Radius = .6f;
	ColliderDesc.pNode = m_pModelCom->Get_HierarchyNode(L"Root");
	ColliderDesc.vOffsetPosition = _float3(0.f, 0.5f, 0.f);
	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::BODY, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}


CNpc_Woman_0* CNpc_Woman_0::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNpc_Woman_0* pInstance = new CNpc_Woman_0(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CNpc_Woman_0");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
	return S_OK;
}

CGameObject* CNpc_Woman_0::Clone(void* pArg)
{
	CNpc_Woman_0* pInstance = new CNpc_Woman_0(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNpc_Woman_0");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNpc_Woman_0::Free()
{
	__super::Free();
}

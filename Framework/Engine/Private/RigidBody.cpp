#include "RigidBody.h"
#include "GameInstance.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
	
}

CRigidBody::CRigidBody(CRigidBody& rhs)
	: CComponent(rhs)
{
}

HRESULT CRigidBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	RIGID_BODY_DESC* pDesc = static_cast<RIGID_BODY_DESC*>(pArg);
	m_pNavigationCom = pDesc->pNavigation;
	m_pTransformCom = pDesc->pTransform;


	if (nullptr == m_pTransformCom)
		return E_FAIL;

	Safe_AddRef(m_pNavigationCom);
	Safe_AddRef(m_pTransformCom);

	return S_OK;
}

void CRigidBody::LateTick_RigidBody(_float fTimeDelta)
{
	if (nullptr != m_pNavigationCom)
		m_fRefHeight = m_pNavigationCom->Compute_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (m_fRefHeight <= XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))	
		m_bIsGround = false;


	Update_Gravity(fTimeDelta);
	Update_Velocity(fTimeDelta);
}
void CRigidBody::Set_PushVelocity(_fvector vPushVelocity, _float fTimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fPushPower = XMVectorGetX(XMVector3Length(vPushVelocity));

	vPosition += XMVector3Normalize(vPushVelocity) * fPushPower;

	m_pTransformCom->Set_Position(vPosition, fTimeDelta, m_pNavigationCom);
}

void CRigidBody::Add_Velocity(_fvector vVelocity)
{
	XMStoreFloat3(&m_vVelocity, XMVectorSet(0.f, 0.f, 0.f, 0.f));
	XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + vVelocity);
}

void CRigidBody::Add_Velocity(_fvector vDir, _float fForce)
{
	XMStoreFloat3(&m_vVelocity, XMVectorSet(0.f, 0.f, 0.f, 0.f));
	XMStoreFloat3(&m_vVelocity, XMVector3Normalize(vDir) * fForce);
}

void CRigidBody::Add_Velocity_Acc(_fvector vVelocity)
{
	XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + vVelocity);
}

void CRigidBody::Add_Velocity_Acc(_fvector vDir, _float fForce)
{
	XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + (XMVector3Normalize(vDir) * fForce));
}


void CRigidBody::Update_Gravity(_float fTimeDelta)
{
	if (!m_bIsGravity)
		return;

	_float4 vPosition;
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (fabs(m_fRefHeight - vPosition.y) <= 0.01f)
	{
		m_bIsGround = true;

		if (m_vVelocity.y > 0.f)
			m_vVelocity.y = 0.f;
	}
	else
	{
		m_bIsGround = false;
		m_vVelocity.y -= m_fGravity * fTimeDelta;
	}
}

void CRigidBody::Update_Velocity(_float fTimeDelta)
{


	_float fVelocityScale = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vVelocity)));
	if (fVelocityScale > 0.0001f)
	{
		_vector vFrictionDir = XMVector3Normalize(XMLoadFloat3(&m_vVelocity) * -1.f);
		_vector vFriction = XMVectorSet(0.f, 0.f, 0.f, 0.f);

		if (Is_Ground())
			vFriction = vFrictionDir * m_fFrictionScale * fTimeDelta;
		else
			vFriction = vFrictionDir * 5.f * fTimeDelta;

		if (fVelocityScale <= XMVectorGetX(XMVector3Length(vFriction)))		
			XMStoreFloat3(&m_vVelocity, XMVectorSet(0.f, 0.f, 0.f, 0.f));
		else		
			XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + vFriction);

		if (m_vVelocity.x >= m_vMaxVelocity.x)
			m_vVelocity.x = m_vMaxVelocity.x;

		if (m_vVelocity.y >= m_vMaxVelocity.y)
			m_vVelocity.y = m_vMaxVelocity.y;

		if (m_vVelocity.z >= m_vMaxVelocity.z)
			m_vVelocity.z = m_vMaxVelocity.z;
	}


	fVelocityScale = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vVelocity)));
	if (fVelocityScale > 0.0001f)
	{
		_vector vDir = XMVector3Normalize(XMLoadFloat3(&m_vVelocity));

		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
		vPosition += XMLoadFloat3(&m_vVelocity) * fTimeDelta;

		if (XMVectorGetY(vPosition) <= m_fRefHeight)
		{
			vPosition = XMVectorSetY(vPosition, m_fRefHeight);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}

		_bool bMovable = false;
		m_pTransformCom->Set_Position(vPosition, fTimeDelta, m_pNavigationCom, &bMovable);
		if (nullptr != m_pNavigationCom && false == bMovable)
		{
			m_vVelocity.x = 0.f;
			m_vVelocity.z = 0.f;
		}
	}


}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidBody* pInstance = new CRigidBody(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Crate Failed. : CRigidBody");
		return nullptr;
	}
	return pInstance;
}

CComponent* CRigidBody::Clone(void* pArg)
{
	CRigidBody* pInstance = new CRigidBody(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Clone Failed. : CRigidBody");
		return nullptr;
	}

	return pInstance;
}

void CRigidBody::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNavigationCom);
}

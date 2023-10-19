#include "RigidBody.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

CRigidBody::CRigidBody(CRigidBody& rhs)
	: CComponent(rhs)
{
}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTransform* pTransform)
{
	return nullptr;
}

CComponent* CRigidBody::Clone(void* pArg)
{
	return nullptr;
}

void CRigidBody::Free()
{
}

#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"


USING(Engine)
CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
	, m_vRotation(rhs.m_vRotation)
{

}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	WRITE_LOCK
	_matrix	WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	WorldMatrix.r[eState] = vState;

	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

HRESULT CTransform::Initialize_Prototype()
{
	/* vector -> float : XMStore*/
	/* float -> vector : XMLoad */

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());	

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResources(CShader* pShader, const char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}




void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (pNavigation == nullptr)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
	{
		if (true == pNavigation->Is_Movable(vPosition))
			Set_State(CTransform::STATE_POSITION, vPosition);
		else
		{
			// TODO :: �����̵� ����
		}
	}
}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (pNavigation == nullptr)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
	{
		if (true == pNavigation->Is_Movable(vPosition))
			Set_State(CTransform::STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (pNavigation == nullptr)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
	{
		if (true == pNavigation->Is_Movable(vPosition))
			Set_State(CTransform::STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (pNavigation == nullptr)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
	{
		if (true == pNavigation->Is_Movable(vPosition))
			Set_State(CTransform::STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Up(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vPosition += XMVector3Normalize(vUp) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (pNavigation == nullptr)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
	{
		if (true == pNavigation->Is_Movable(vPosition))
			Set_State(CTransform::STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Down(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vPosition -= XMVector3Normalize(vUp) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (pNavigation == nullptr)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
	{
		if (true == pNavigation->Is_Movable(vPosition))
			Set_State(CTransform::STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Dir(_fvector vDir, _float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (pNavigation == nullptr)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
	{
		if (true == pNavigation->Is_Movable(vPosition))
			Set_State(CTransform::STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	vPosition += XMVector3Normalize(vDir) * fSpeed * fTimeDelta;

	if(pNavigation == nullptr)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
	{
		if(true == pNavigation->Is_Movable(vPosition))
			Set_State(CTransform::STATE_POSITION, vPosition);
	}
}

void CTransform::Set_Scale(_fvector vScaleInfo)
{
	WRITE_LOCK

	Set_State(CTransform::STATE_RIGHT, 
		XMVector3Normalize(Get_State(CTransform::STATE_RIGHT)) * XMVectorGetX(vScaleInfo));
	Set_State(CTransform::STATE_UP, 
		XMVector3Normalize(Get_State(CTransform::STATE_UP)) * XMVectorGetY(vScaleInfo));
	Set_State(CTransform::STATE_LOOK, 
		XMVector3Normalize(Get_State(CTransform::STATE_LOOK)) * XMVectorGetZ(vScaleInfo));	
}

void CTransform::Set_Sclae(_float3 vScale)
{
	WRITE_LOCK

	Set_State(CTransform::STATE_RIGHT, 
		XMVector3Normalize(Get_State(CTransform::STATE_RIGHT)) * vScale.x);
	Set_State(CTransform::STATE_UP, 
		XMVector3Normalize(Get_State(CTransform::STATE_UP)) * vScale.y);
	Set_State(CTransform::STATE_LOOK, 
		XMVector3Normalize(Get_State(CTransform::STATE_LOOK)) * vScale.z);
}

_float3 CTransform::Get_Scale()
{
	READ_LOCK

	return _float3(
		XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_RIGHT))),
		XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_UP))),
		XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_LOOK))));
}

_float3 CTransform::Get_Rotaion_Degree()
{	
	READ_LOCK

	_vector vScale;
	_vector vQuaternion;
	_vector vPostion;

	XMMatrixDecompose(&vScale, &vQuaternion, &vPostion, XMLoadFloat4x4(&m_WorldMatrix));

	_float3 fAngle = ToEulerAngles(vQuaternion);
	fAngle.x = XMConvertToDegrees(fAngle.x);
	fAngle.y = XMConvertToDegrees(fAngle.y);
	fAngle.z = XMConvertToDegrees(fAngle.z);

	return fAngle;
}

_float3 CTransform::Get_Rotation_Radian()
{
	READ_LOCK

	_vector vScale;
	_vector vQuaternion;
	_vector vPostion;

	XMMatrixDecompose(&vScale, &vQuaternion, &vPostion, XMLoadFloat4x4(&m_WorldMatrix));

	_float3 fAngle = ToEulerAngles(vQuaternion);

	return fAngle;
}

void CTransform::Set_Rotation(_fvector vRadianEulerAngle)
{
	// �������� ���� ���ʹϿ����� ȸ��.
	_float3 vRotation;
	XMStoreFloat3(&vRotation, vRadianEulerAngle);

	_matrix RotationMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(vRotation.x, vRotation.y, vRotation.z));


	_float3		Scale = Get_Scale();

	WRITE_LOCK

	if (vRotation.y <= 91.f && vRotation.y >= 89.f)
	{
		Set_State(CTransform::STATE_UP, XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f) * Scale.y, RotationMatrix));
		Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f) * Scale.x, RotationMatrix));
		Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f) * Scale.z, RotationMatrix));
	}
	else
	{
		Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f) * Scale.x, RotationMatrix));
		Set_State(CTransform::STATE_UP, XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f) * Scale.y, RotationMatrix));
		Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f) * Scale.z, RotationMatrix));
	}
	
	
	
	
	
}

void CTransform::Set_Position(_fvector vPosition, CNavigation* pNavigation)
{
	if (pNavigation == nullptr)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
	{
		if (true == pNavigation->Is_Movable(vPosition))
		{
			Set_State(CTransform::STATE_POSITION, vPosition);
		}
		else
		{
			_vector vTransformPosition = Get_State(CTransform::STATE_POSITION);
			vTransformPosition = XMVectorSetY(vTransformPosition, XMVectorGetY(vPosition));
			Set_State(CTransform::STATE_POSITION, vTransformPosition);
		}
			
	}

}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * fTimeDelta);
	
	WRITE_LOCK

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(Get_State(CTransform::STATE_RIGHT), RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(Get_State(CTransform::STATE_UP), RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(Get_State(CTransform::STATE_LOOK), RotationMatrix));
}

void CTransform::Turn(_fvector vAxis, _float fSpeed, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fSpeed * fTimeDelta);

	WRITE_LOCK

		Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(Get_State(CTransform::STATE_RIGHT), RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(Get_State(CTransform::STATE_UP), RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(Get_State(CTransform::STATE_LOOK), RotationMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	// Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(60.0f));

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	_float3		Scale = Get_Scale();

	WRITE_LOCK

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f) * Scale.x, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f) * Scale.y, RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f) * Scale.z, RotationMatrix));

}

void CTransform::Rotation_Acc(_fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	WRITE_LOCK

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(Get_State(CTransform::STATE_RIGHT), RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(Get_State(CTransform::STATE_UP), RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(Get_State(CTransform::STATE_LOOK), RotationMatrix));
}

void CTransform::LookAt(_fvector vAt)
{
	_vector		vLook = vAt - Get_State(CTransform::STATE_POSITION);

	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScale = Get_Scale();

	WRITE_LOCK

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_ForLandObject(_fvector vAt)
{
	_vector		vLook = vAt - Get_State(CTransform::STATE_POSITION);

	_float3		vScale = Get_Scale();

	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;	

	vLook = XMVector3Normalize(XMVector3Cross(vRight, Get_State(CTransform::STATE_UP))) * vScale.z;



	WRITE_LOCK
	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_LOOK, vLook);

}

void CTransform::Move(_fvector vTargetPos, _float fTimeDelta, _float fLimitDistance)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vDirection = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDirection));

	if(fDistance > fLimitDistance)
		vPosition += XMVector3Normalize(vDirection) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	WRITE_LOCK

	Set_State(CTransform::STATE_POSITION, vPosition);
}

_float3 CTransform::ToEulerAngles(_vector Quaternion)
{
	READ_LOCK
	_float3 vAngles;
	ZeroMemory(&vAngles, sizeof(_float3));

	_float4 vQuaternion;

	XMStoreFloat4(&vQuaternion, Quaternion);


	// roll (x-axis rotation)
	_float sinr_cosp = 2 * (vQuaternion.w * vQuaternion.x + vQuaternion.y * vQuaternion.z);
	_float cosr_cosp = 1 - 2 * (vQuaternion.x * vQuaternion.x + vQuaternion.y * vQuaternion.y);
	vAngles.x = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	_float sinp = std::sqrt(1 + 2 * (vQuaternion.w * vQuaternion.y - vQuaternion.x * vQuaternion.z));
	_float cosp = std::sqrt(1 - 2 * (vQuaternion.w * vQuaternion.y - vQuaternion.x * vQuaternion.z));
	vAngles.y = 2 * std::atan2(sinp, cosp) - 3.14159f / 2;

	// yaw (z-axis rotation)
	_float siny_cosp = 2 * (vQuaternion.w * vQuaternion.z + vQuaternion.x * vQuaternion.y);
	_float cosy_cosp = 1 - 2 * (vQuaternion.y * vQuaternion.y + vQuaternion.z * vQuaternion.z);
	vAngles.z = std::atan2(siny_cosp, cosy_cosp);

	return vAngles;
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*			pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*			pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CTransform::Free()
{
	__super::Free();
}




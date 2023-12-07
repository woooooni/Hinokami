#include "stdafx.h"
#include "..\Public\Camera_Main.h"
#include "GameInstance.h"
#include "Key_Manager.h"
#include "Utils.h"

CCamera_Main::CCamera_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, wstring strObjTag)
	: CCamera(pDevice, pContext, strObjTag, OBJ_TYPE::OBJ_CAMERA)
{
}

CCamera_Main::CCamera_Main(const CCamera_Main & rhs, CTransform::TRANSFORMDESC* pArg)
	: CCamera(rhs)
{

}

HRESULT CCamera_Main::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Main::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	

	return S_OK;
}

void CCamera_Main::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	__super::LateTick(fTimeDelta);
}

void CCamera_Main::LateTick(_float fTimeDelta)
{
	if (false == m_tShakeDesc.bEnd)
	{
		m_tShakeDesc.fAccTime += fTimeDelta;
		if (m_tShakeDesc.fAccTime >= m_tShakeDesc.fDuration)
			m_tShakeDesc.bEnd = true;

		_float fForce = CUtils::Random_Float(-m_tShakeDesc.fForce, m_tShakeDesc.fForce);

		_vector vShakeDir = XMVector3Normalize(XMVectorSet(1.f, 1.f, 1.f, 0.f));
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPosition += vShakeDir * fForce * fTimeDelta;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}

	
	
}

HRESULT CCamera_Main::Render()
{
	return S_OK;
}

void CCamera_Main::Tick_Basic(_float fTimeDelta)
{
	_long	MouseMove = 0;


	if (MouseMove = GI->Get_DIMMoveState(DIMM_Y))
	{
		m_vAngle.x -= MouseMove * fTimeDelta * 2.f;
		if (360.f <= m_vAngle.x)
			m_vAngle.x = 0.f;
		else if (0.f >= m_vAngle.x)
			m_vAngle.x = 360.f;
	}

	if (MouseMove = GI->Get_DIMMoveState(DIMM_X))
	{
		m_vAngle.y += MouseMove * fTimeDelta * 5.f;
		if (360.f <= m_vAngle.y)
			m_vAngle.y = 0.f;
		else if (0.f >= m_vAngle.y)
			m_vAngle.y = 360.f;
	}

	// x, y 회전 행렬
	_matrix mX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_vAngle.x));
	_matrix mY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_vAngle.y));

	// 멀어질 방향
	_vector vCamDir = XMVector3Normalize(XMVectorSet(0.f, 1.f, -1.f, 0.f));

	// X, Y회전
	vCamDir = XMVector3TransformNormal(vCamDir, mX);
	vCamDir = XMVector3TransformNormal(vCamDir, mY);
	_vector vCamPos = vCamDir * m_fOffsetDistance;

	_vector vPlayerPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vDestPos = vPlayerPos + vCamPos;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), vDestPos, m_fCamSpeed * fTimeDelta));

	_float4 vLookAt;
	XMStoreFloat4(&vLookAt, vPlayerPos);
	vLookAt.y += 1.f;
	m_pTransformCom->LookAt(XMLoadFloat4(&vLookAt));
}

HRESULT CCamera_Main::Ready_Components()
{
	__super::Ready_Components();


	if (FAILED(__super::Add_Component(L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Main::Follow(_float fTimeDelta)
{
	_vector vTargetPosition = m_pTargetTransform->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vOffsetPosition);;
	_vector vCamPostion = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorLerp(vTargetPosition, vCamPostion, fTimeDelta));
	m_pTransformCom->LookAt(m_pTargetTransform->Get_State(CTransform::STATE_POSITION));
}

CCamera_Main * CCamera_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, wstring strObjTag)
{
	CCamera_Main*		pInstance = new CCamera_Main(pDevice, pContext, strObjTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCamera_Main");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Main::Clone(void* pArg)
{
	CCamera::CAMERADESC*		pCameraDesc = (CCamera::CAMERADESC*)pArg;

	CCamera_Main*		pInstance = new CCamera_Main(*this, &pCameraDesc->TransformDesc);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Created : CCamera_Main");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Main::Free()
{
	__super::Free();


}

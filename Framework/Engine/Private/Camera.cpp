#include "..\Public\Camera.h"
#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, wstring strObjTag, _uint iObjectID)
	: CGameObject(pDevice, pContext, strObjTag, iObjectID)
{
}

CCamera::CCamera(const CCamera & rhs, CTransform::TRANSFORMDESC* pArg)
	: CGameObject(rhs)
	, m_pTransformCom((CTransform*)rhs.m_pTransformCom->Clone(pArg))
{

}

HRESULT CCamera::Initialize_Prototype()
{
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;


	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_CameraDesc.vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&m_CameraDesc.vAt));

	ZeroMemory(&m_tShakeDesc, sizeof(CAM_SHAKE));
	m_tShakeDesc.bEnd = true;

	return S_OK;
}

void CCamera::Tick(_float fTimeDelta)
{



}

void CCamera::LateTick(_float fTimeDelta)
{
	GI->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrixInverse());
	GI->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));
}

HRESULT CCamera::Render()
{
	return S_OK;
}



void CCamera::Cam_Shake(const CAM_SHAKE& pCameraShakeDesc)
{
	m_tShakeDesc = pCameraShakeDesc;

	m_tShakeDesc.fForce = max(m_tShakeDesc.fForce, 1.f);
	m_tShakeDesc.bEnd = false;
	m_tShakeDesc.fAccTime = 0.f;
}

HRESULT CCamera::Ready_Components()
{
	return S_OK;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
}

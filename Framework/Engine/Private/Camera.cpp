#include "..\Public\Camera.h"
#include "PipeLine.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, wstring strObjTag)
	: CGameObject(pDevice, pContext, strObjTag)
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

	return S_OK;
}

void CCamera::Tick(_float fTimeDelta)
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();

	pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrixInverse());
	pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));
}

void CCamera::LateTick(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
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

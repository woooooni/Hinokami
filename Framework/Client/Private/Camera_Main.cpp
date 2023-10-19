#include "stdafx.h"
#include "..\Public\Camera_Main.h"
#include "GameInstance.h"
#include "Key_Manager.h"

CCamera_Main::CCamera_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, wstring strObjTag)
	: CCamera(pDevice, pContext, strObjTag, OBJ_TYPE::OBJ_CAMERA)
{
}

CCamera_Main::CCamera_Main(const CCamera_Main & rhs, CTransform::TRANSFORMDESC* pArg)
	: CCamera(rhs, pArg)
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
	
	_vector vCameraRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector vCameraUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vCameraLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vCameraPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_long	MouseMove = 0;



	//MouseMove = 0;
	//if (MouseMove = GI->Get_DIMMoveState(DIMM_Y))
	//{
	//	_float fMoveDir = clamp(_float(MouseMove), -1.f, 1.f);
	//	m_pTransformCom->Rotation_Acc(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fMoveDir * fTimeDelta);
	//}



	

	

	__super::Tick(fTimeDelta);
}

void CCamera_Main::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CCamera_Main::Render()
{
	return S_OK;
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

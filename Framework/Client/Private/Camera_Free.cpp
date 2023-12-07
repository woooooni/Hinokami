#include "stdafx.h"
#include "..\Public\Camera_Free.h"
#include "GameInstance.h"
#include "Key_Manager.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, wstring strObjTag)
	: CCamera(pDevice, pContext, strObjTag, OBJ_TYPE::OBJ_CAMERA)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free & rhs)
	: CCamera(rhs)
{

}

HRESULT CCamera_Free::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Free::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	

	return S_OK;
}

void CCamera_Free::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (KEY_HOLD(KEY::RBTN))
	{
		if (KEY_HOLD(KEY::W))
		{
			m_pTransformCom->Go_Straight(fTimeDelta * 10.f);
		}

		if (KEY_HOLD(KEY::S))
		{
			m_pTransformCom->Go_Straight(fTimeDelta * -10.f);
		}

		if (KEY_HOLD(KEY::A))
		{
			m_pTransformCom->Go_Dir(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)), fTimeDelta * -10.f);
		}

		if (KEY_HOLD(KEY::D))
		{
			m_pTransformCom->Go_Dir(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)), fTimeDelta * 10.f);
		}

		if (KEY_HOLD(KEY::Q))
		{
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 10.f);
		}

		if (KEY_HOLD(KEY::E))
		{
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -10.f);
		}

		if (KEY_TAP(KEY::R))
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_CameraDesc.vEye));
			m_pTransformCom->LookAt(XMLoadFloat4(&m_CameraDesc.vAt));
		}

		
		_long	MouseMove = 0;

		if (MouseMove = GAME_INSTANCE->Get_DIMMoveState(DIMM_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * 0.05f);
		}

		if (MouseMove = GAME_INSTANCE->Get_DIMMoveState(DIMM_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * 0.05f);
		}
	}

	__super::LateTick(fTimeDelta);
	
}

void CCamera_Free::LateTick(_float fTimeDelta)
{
	// __super::LateTick(fTimeDelta);
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

HRESULT CCamera_Free::Ready_Components()
{
	__super::Ready_Components();


	if (FAILED(__super::Add_Component(L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

CCamera_Free * CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, wstring strObjTag)
{
	CCamera_Free*		pInstance = new CCamera_Free(pDevice, pContext, strObjTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Free::Clone(void* pArg)
{
	CCamera::CAMERADESC*		pCameraDesc = (CCamera::CAMERADESC*)pArg;

	CCamera_Free*		pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Created : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();


}

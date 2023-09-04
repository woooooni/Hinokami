#include "stdafx.h"
#include "..\Public\Camera_Free.h"
#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free & rhs, CTransform::TRANSFORMDESC * pArg)
	: CCamera(rhs, pArg)
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
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	

	return S_OK;
}

void CCamera_Free::Tick(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (GetAsyncKeyState('W') & 0x8001)
	{
		m_pTransformCom->Go_Straight(fTimeDelta * 100.f);
	}

	if (GetAsyncKeyState('S') & 0x8001)
	{
		m_pTransformCom->Go_Backward(fTimeDelta * 100.f);
	}

	if (GetAsyncKeyState('A') & 0x8001)
	{
		m_pTransformCom->Go_Left(fTimeDelta * 100.f);
	}

	if (GetAsyncKeyState('D') & 0x8001)
	{
		m_pTransformCom->Go_Right(fTimeDelta * 100.f);
	}

	if (GetAsyncKeyState('R') & 0x8000)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_CameraDesc.vEye));
		m_pTransformCom->LookAt(XMLoadFloat4(&m_CameraDesc.vAt));
	}

	//_long	MouseMove = 0;

	//if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_X))
	//{
	//	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * 0.05f);		
	//}

	//if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_Y))
	//{
	//	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * 0.05f);
	//}

	

	Safe_Release(pGameInstance);

	__super::Tick(fTimeDelta);



}

void CCamera_Free::LateTick(_float fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

HRESULT CCamera_Free::Ready_Components()
{
	__super::Ready_Components();
	return S_OK;
}

CCamera_Free * CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free*		pInstance = new CCamera_Free(pDevice, pContext);

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

	CCamera_Free*		pInstance = new CCamera_Free(*this, &pCameraDesc->TransformDesc);

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

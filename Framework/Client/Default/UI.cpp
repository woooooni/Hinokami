#include "stdafx.h"
#include "UI.h"
#include "GameInstance.h"



CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
	: CGameObject(pDevice, pContext, strObjectTag, OBJ_TYPE::OBJ_UI)
{
}

CUI::CUI(const CUI& rhs)
	: CGameObject(rhs)
	, m_tInfo(rhs.m_tInfo)
	, m_ViewMatrix(rhs.m_ViewMatrix)
	, m_ProjMatrix(rhs.m_ProjMatrix)
{
}

void CUI::Debug_Input(_float fTimeDelta)
{
	if (KEY_TAP(KEY::LEFT_ARROW))
		m_tInfo.fX -= 1.f;

	if(KEY_TAP(KEY::RIGHT_ARROW))
		m_tInfo.fX += 1.f;

	if (KEY_TAP(KEY::UP_ARROW))
		m_tInfo.fY += 1.f;

	if (KEY_TAP(KEY::DOWN_ARROW))
		m_tInfo.fY -= 1.f;

	if (KEY_TAP(KEY::OPEN_SQUARE_BRACKET))
		m_tInfo.fCX -= 1.f;

	if (KEY_TAP(KEY::CLOSE_SQUARE_BRACKET))
		m_tInfo.fCX += 1.f;

	if(KEY_TAP(KEY::O))
		m_tInfo.fCY -= 1.f;

	if (KEY_TAP(KEY::P))
		m_tInfo.fCY += 1.f;



	m_pTransformCom->Set_Scale(XMLoadFloat3(&_float3(m_tInfo.fCX, m_tInfo.fCY, 1.f)));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_tInfo.fX - g_iWinSizeX * 0.5f, -m_tInfo.fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
}

void CUI::Free()
{
	__super::Free();
}

//  m_pTransformCom->Set_State(CTransform::STATE_POSITION,
//	XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
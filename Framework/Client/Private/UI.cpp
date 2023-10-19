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
	, m_strText(rhs.m_strText)
{
}

void CUI::Tick(_float fTimeDelta)
{
	POINT pt = GI->GetMousePos();
	RECT MouseRect;
	MouseRect.left = pt.x - 1.f;
	MouseRect.right = pt.x + 1.f;
	MouseRect.top = pt.y - 1.f;
	MouseRect.bottom = pt.y + 1.f;

	RECT UIRect;
	UIRect.left = m_tInfo.fX - m_tInfo.fCX / 2.f;
	UIRect.right = m_tInfo.fX + m_tInfo.fCX / 2.f;
	UIRect.top = m_tInfo.fY - m_tInfo.fCY / 2.f;
	UIRect.bottom = m_tInfo.fY + m_tInfo.fCY / 2.f;

	RECT Result;
	if (IntersectRect(&Result, &MouseRect, &UIRect))
	{
		// 들어와 있다.
		switch(m_eMouseState)
		{
		case UI_MOUSESTATE::MOUSE_ENTER:
			m_eMouseState = UI_MOUSESTATE::MOUSE_ON;
			On_Mouse(fTimeDelta);
			break;
		case UI_MOUSESTATE::MOUSE_ON:
			On_Mouse(fTimeDelta);
			break;
		case UI_MOUSESTATE::MOUSE_EXIT:
			m_eMouseState = UI_MOUSESTATE::MOUSE_ENTER;
			On_MouseEnter(fTimeDelta);
			break;
		default:
			m_eMouseState = UI_MOUSESTATE::MOUSE_ENTER;
			On_MouseEnter(fTimeDelta);
			break;

		}
	}
	else
	{
		// 안들어와 있다.
		switch (m_eMouseState)
		{
		case UI_MOUSESTATE::MOUSE_ENTER:
			m_eMouseState = UI_MOUSESTATE::MOUSE_EXIT;
			On_MouseExit(fTimeDelta);
			break;
		case UI_MOUSESTATE::MOUSE_ON:
			m_eMouseState = UI_MOUSESTATE::MOUSE_EXIT;
			On_MouseExit(fTimeDelta);
			break;
		case UI_MOUSESTATE::MOUSE_EXIT:
			m_eMouseState = UI_MOUSESTATE::MOUSE_END;
			break;
		default:
			break;
		}
	}

}

void CUI::Debug_Input(_float fTimeDelta)
{
	if (KEY_TAP(KEY::LEFT_ARROW))
		m_tInfo.fX -= 10.f;

	if(KEY_TAP(KEY::RIGHT_ARROW))
		m_tInfo.fX += 10.f;

	if (KEY_TAP(KEY::UP_ARROW))
		m_tInfo.fY += 10.f;

	if (KEY_TAP(KEY::DOWN_ARROW))
		m_tInfo.fY -= 10.f;

	if (KEY_TAP(KEY::OPEN_SQUARE_BRACKET))
		m_tInfo.fCX -= 10.f;

	if (KEY_TAP(KEY::CLOSE_SQUARE_BRACKET))
		m_tInfo.fCX += 10.f;

	if(KEY_TAP(KEY::O))
		m_tInfo.fCY -= 10.f;

	if (KEY_TAP(KEY::P))
		m_tInfo.fCY += 10.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_tInfo.fX - g_iWinSizeX * 0.5f, -m_tInfo.fY + g_iWinSizeY * 0.5f, 1.f, 1.f));
	m_pTransformCom->Set_Scale(XMLoadFloat3(&_float3(m_tInfo.fCX, m_tInfo.fCY, 1.f)));
}

void CUI::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

}

//  m_pTransformCom->Set_State(CTransform::STATE_POSITION,
//	XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
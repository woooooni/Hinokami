#include "stdafx.h"
#include "..\Public\UI_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Effect.h"
#include <filesystem>
#include "FileUtils.h"
#include "Utils.h"
#include "UI_Cursor.h"
#include "UI_GaugeBar.h"

#include "UI_BattleStart.h"
#include "UI_BattleEnd.h"

#include "Character.h"
#include "Monster.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{

}

HRESULT CUI_Manager::Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	CUI::UI_INFO Info;
	ZeroMemory(&Info, sizeof(CUI::UI_INFO));

	Info.fX = g_iWinSizeX / 2.f;
	Info.fY = g_iWinSizeY / 2.f;

	Info.fCX = 60.f;
	Info.fCY = 72.f;
	m_pCursor = CUI_Cursor::Create(m_pDevice, m_pContext, Info);

	if (nullptr == m_pCursor)
		return E_FAIL;

	if (FAILED(m_pCursor->Initialize(nullptr)))
		return E_FAIL;


	for (_uint i = 0; i < GAUGE_BARTYPE::TYPE_END; ++i)
	{
		switch (i)
		{
		case GAUGE_BARTYPE::LEFT_HP :
			m_pGaugeBars[i] = CUI_GaugeBar::Create(m_pDevice, m_pContext, CUI_GaugeBar::GAUGE_TYPE::HP, CUI_GaugeBar::GAUGE_BAR_POSITION::LEFT_TOP);
			if (FAILED(m_pGaugeBars[i]->Initialize(nullptr)))
				return E_FAIL;
			break;

		case GAUGE_BARTYPE::RIGHT_HP:
			m_pGaugeBars[i] = CUI_GaugeBar::Create(m_pDevice, m_pContext, CUI_GaugeBar::GAUGE_TYPE::HP, CUI_GaugeBar::GAUGE_BAR_POSITION::RIGHT_TOP);
			if (FAILED(m_pGaugeBars[i]->Initialize(nullptr)))
				return E_FAIL;
			break;

		case GAUGE_BARTYPE::LEFT_MP:
			m_pGaugeBars[i] = CUI_GaugeBar::Create(m_pDevice, m_pContext, CUI_GaugeBar::GAUGE_TYPE::MP, CUI_GaugeBar::GAUGE_BAR_POSITION::LEFT_TOP_BOTTOM);
			if (FAILED(m_pGaugeBars[i]->Initialize(nullptr)))
				return E_FAIL;
			break;

		case GAUGE_BARTYPE::RIGHT_MP:
			m_pGaugeBars[i] = CUI_GaugeBar::Create(m_pDevice, m_pContext, CUI_GaugeBar::GAUGE_TYPE::MP, CUI_GaugeBar::GAUGE_BAR_POSITION::RIGHT_TOP_BOTTOM);
			if (FAILED(m_pGaugeBars[i]->Initialize(nullptr)))
				return E_FAIL;
			break;

		}
	}

	Info.fX = g_iWinSizeX / 2.f;
	Info.fY = g_iWinSizeY / 2.f;

	Info.fCX = 692.f;
	Info.fCY = 260.f;

	if (FAILED(GI->Add_Prototype(L"UI_BattleStart", CUI_BattleStart::Create(m_pDevice, m_pContext, Info), LAYER_UI)))
		return E_FAIL;


	Info.fCX = 1000.f;
	Info.fCY = 260.f;
	if (FAILED(GI->Add_Prototype(L"UI_BattleEnd", CUI_BattleEnd::Create(m_pDevice, m_pContext, Info), LAYER_UI)))
		return E_FAIL;



	return S_OK;
}


void CUI_Manager::Tick(_float fTimeDelta)
{
	m_pCursor->Tick(fTimeDelta);
	for (_uint i = 0; i < GAUGE_BARTYPE::TYPE_END; ++i)
	{
		if (nullptr != m_pGaugeBars[i])
			m_pGaugeBars[i]->Tick(fTimeDelta);
	}

}

void CUI_Manager::LateTick(_float fTimeDelta)
{
	m_pCursor->LateTick(fTimeDelta);

	for (_uint i = 0; i < GAUGE_BARTYPE::TYPE_END; ++i)
	{
		if (nullptr != m_pGaugeBars[i])
			m_pGaugeBars[i]->LateTick(fTimeDelta);
	}
}

HRESULT CUI_Manager::Battle_Start()
{
	if (FAILED(GI->Add_GameObject(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_UI, L"UI_BattleStart")))
		return E_FAIL;

	// TODO:: Play Sound
	return S_OK;
}

HRESULT CUI_Manager::Battle_End()
{
	if (FAILED(GI->Add_GameObject(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_UI, L"UI_BattleEnd")))
		return E_FAIL;
	
	// TODO:: Play Sound
	return S_OK;
}

HRESULT CUI_Manager::Reset_HpBar(GAUGE_BARTYPE eBarType)
{
	if (eBarType >= GAUGE_BARTYPE::TYPE_END)
		return E_FAIL;

	m_pGaugeBars[eBarType]->Reset();
	return S_OK;
}

HRESULT CUI_Manager::Reserve_HpBar(GAUGE_BARTYPE eBarType, CCharacter* pCharacter, _uint iCharacterType)
{
	if (eBarType >= GAUGE_BARTYPE::TYPE_END)
		return E_FAIL;

	if (iCharacterType >= CUI_GaugeBar::CHARACTER_TYPE::CHARACTER_END)
		return E_FAIL;

	m_pGaugeBars[eBarType]->Set_Owner(pCharacter, CUI_GaugeBar::CHARACTER_TYPE(iCharacterType));

	return S_OK;
}

HRESULT CUI_Manager::Reserve_HpBar(GAUGE_BARTYPE eBarType, CMonster* pMonster, _uint iCharacterType)
{
	if (eBarType >= GAUGE_BARTYPE::TYPE_END)
		return E_FAIL;

	if (iCharacterType >= CUI_GaugeBar::CHARACTER_TYPE::CHARACTER_END)
		return E_FAIL;

	m_pGaugeBars[eBarType]->Set_Owner(pMonster, CUI_GaugeBar::CHARACTER_TYPE(iCharacterType));
	return S_OK;
}




void CUI_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pCursor);

	for (_uint i = 0; i < GAUGE_BARTYPE::TYPE_END; ++i)
	{
		if (nullptr != m_pGaugeBars[i])
			Safe_Release(m_pGaugeBars[i]);
	}
}

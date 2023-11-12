#include "..\Public\Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

void CLevel_Manager::Tick(_float fTimeDelta)
{
	if (nullptr != m_pCurrentLevel)
		m_pCurrentLevel->Tick(fTimeDelta);
}

void CLevel_Manager::LateTick(_float fTimeDelta)
{
	if (nullptr != m_pCurrentLevel)
		m_pCurrentLevel->LateTick(fTimeDelta);

	
}

HRESULT CLevel_Manager::Render_Debug()
{
	
	if (nullptr != m_pCurrentLevel)
		m_pCurrentLevel->Render_Debug();


	if (nullptr != m_pReserveLevel)
	{
		WRITE_LOCK
		if (nullptr != m_pCurrentLevel)
		{
			m_pCurrentLevel->Exit_Level();
			/* 기존에 추가되어있던 레벨용 자원을 정리한다. */
			GI->Clear(m_iCurrentLevelIndex);

		}
		
		m_pCurrentLevel = m_pReserveLevel;


		m_iCurrentLevelIndex = m_iNextLevelIndex;

		m_pReserveLevel = nullptr;
		m_iNextLevelIndex = -1;

		m_pCurrentLevel->Enter_Level();
	}
	return S_OK;
}

HRESULT CLevel_Manager::Open_Level(_uint iLevelIndex, CLevel* pNewLevel)
{		
	WRITE_LOCK
	m_iNextLevelIndex = iLevelIndex;
	m_pReserveLevel = pNewLevel;


	return S_OK;
}

_uint CLevel_Manager::Get_CurrentLevel()
{
	READ_LOCK
	return m_iCurrentLevelIndex;
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pReserveLevel);
	Safe_Release(m_pCurrentLevel);
}

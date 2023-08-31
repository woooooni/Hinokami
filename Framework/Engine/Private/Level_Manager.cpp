#include "..\Public\Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

void CLevel_Manager::Tick(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Tick(fTimeDelta);
}

void CLevel_Manager::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->LateTick(fTimeDelta);
}

HRESULT CLevel_Manager::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr != m_pCurrentLevel)
	{
		CGameInstance*	pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		/* 기존에 추가되어있던 레벨용 자원을 정리한다. */
		pGameInstance->Clear(m_iCurrentLevelIndex);

		Safe_Release(pGameInstance);
	}

	/* 기존레벨 객체를 파굏란다. */
	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNewLevel;

	m_iCurrentLevelIndex = iLevelIndex;

	return S_OK;
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}

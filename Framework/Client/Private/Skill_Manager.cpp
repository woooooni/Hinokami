#include "stdafx.h"
#include "Skill_Manager.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Transform.h"
#include "GameObject.h"
#include "Effect.h"
#include <filesystem>
#include "FileUtils.h"
#include "Utils.h"

IMPLEMENT_SINGLETON(CSkill_Manager)

CSkill_Manager::CSkill_Manager()
{

}

HRESULT CSkill_Manager::Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strEffectPath)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	if (FAILED(Ready_Skills()))
		return E_FAIL;
	
	return S_OK;
}

void CSkill_Manager::Tick(_float fTimeDelta)
{

}

HRESULT CSkill_Manager::Use_Skill(CCharacter* pGameObject, SKILL_TYPE eSkillType)
{

	return S_OK;
}

HRESULT CSkill_Manager::Use_Skill(CMonster* pGameObject, SKILL_TYPE eSkillType)
{

	return S_OK;
}


HRESULT CSkill_Manager::Ready_Skills()
{

	return S_OK;
}

void CSkill_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

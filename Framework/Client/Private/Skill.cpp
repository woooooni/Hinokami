#include "stdafx.h"
#include "GameInstance.h"
#include "Skill.h"


USING(Client)
CSkill::CSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strSkillTag)
	: m_strSkillTag(strSkillTag)
{
}

CSkill::CSkill(const CSkill& rhs)
	: m_strSkillTag(rhs.m_strSkillTag)
{	

}

void CSkill::Free()
{
	__super::Free();

}

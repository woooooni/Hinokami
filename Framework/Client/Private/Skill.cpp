#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Skill.h"


USING(Client)
CSkill::CSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strSkillTag, _float fSkillCoolTime, _uint iSkillCount)
	: m_strSkillTag(strSkillTag)
	, m_fSkillCoolTime(fSkillCoolTime)
	, m_iCurrUseCount(iSkillCount)
	, m_iSkillCount(iSkillCount)
{
}


HRESULT CSkill::Initialize()
{
	return S_OK;
}

void CSkill::Tick(_float fTimeDelta)
{
	if (false == m_bUseAble)
	{
		m_fAccSkillCoolTime += fTimeDelta;
		if (m_fAccSkillCoolTime >= m_fSkillCoolTime)
		{
			m_bUseAble = true;
			m_fAccSkillCoolTime = 0.f;
			m_iCurrUseCount = m_iSkillCount;
		}
	}
}


void CSkill::Find_Near_Target(_float fDistance)
{
	m_pTarget = nullptr;
	_float fMinDistance = fDistance;
	CTransform* pFindTargetTransform = nullptr;
	

	const list<CGameObject*>& Targets = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_MONSTER);
	for (auto& pTarget : Targets)
	{
		CTransform* pOwnerTransform = m_pOwner->Get_Component<CTransform>(L"Com_Transform");
		CTransform* pTargetTransform = pTarget->Get_Component<CTransform>(L"Com_Transform");
		if (nullptr == pTargetTransform || nullptr == pOwnerTransform)
			continue;

		_float fTargetDistance = Vec3::Distance(pTargetTransform->Get_Position(), pOwnerTransform->Get_Position());
		if (fTargetDistance < fMinDistance)
		{
			fMinDistance = fTargetDistance;
			m_pTarget = pTarget;
		}
	}
}

void CSkill::Look_For_Target()
{
	if (nullptr == m_pTarget || m_pTarget->Is_Dead() || m_pTarget->Is_ReserveDead())
		return;

	CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>(L"Com_Transform");
	CTransform* pOwnerTransform = m_pOwner->Get_Component<CTransform>(L"Com_Transform");
	if (nullptr == pTargetTransform || nullptr == pOwnerTransform)
		return;

	pOwnerTransform->LookAt_ForLandObject(pTargetTransform->Get_Position());
}

void CSkill::Free()
{
	__super::Free();

}
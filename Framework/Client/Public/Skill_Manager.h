#pragma once

#include "Client_Defines.h"
#include "Base.h"


BEGIN(Client)
class CSkill_Manager : public CBase
{
	
	DECLARE_SINGLETON(CSkill_Manager)
public:
	enum SKILL_TYPE { SKILL_TANJIRO_0, SKILL_TANJIRO_1, SKILL_TANJIRO_2, SKILL_ZENITSU_0, SKILL_ZENITSU_0_AIR, SKILL_END };

private:
	CSkill_Manager();
	virtual ~CSkill_Manager() = default;

public:
	HRESULT Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strEffectPath);
	void	Tick(_float fTimeDelta);
	HRESULT Use_Skill(class CCharacter* pGameObject, SKILL_TYPE eSkillType);
	HRESULT Use_Skill(class CMonster* pGameObject, SKILL_TYPE eSkillType);

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

	vector<class CSkill*> m_Skills[SKILL_TYPE::SKILL_END];

private:
	HRESULT Ready_Skills();


public:
	virtual void Free() override;
};

END

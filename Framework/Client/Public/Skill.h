#pragma once

#include "Client_Defines.h"
#include "Base.h"
BEGIN(Client)

class CSkill abstract : public CBase
{

protected:
	CSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strSkillTag, _float fSkillCoolTime, _uint iSkillCount);
	virtual ~CSkill() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual void Use_Skill(class CGameObject* pCharacter, _float fTimeDelta) PURE;

protected:
	wstring m_strSkillTag;
	_bool m_bUseAble = true;
	_float m_fSkillCoolTime = 1.f;
	_float m_fAccSkillCoolTime = 0.f;

	_uint m_iSkillCount = 1;
	_uint m_iCurrUseCount = 1;

	class CGameObject* m_pOwner = nullptr;
	class CGameObject* m_pTarget = nullptr;

protected:
	void Find_Near_Target(_float fDistance);
	void Look_For_Target();


public:
	virtual void Free() override;
};

END


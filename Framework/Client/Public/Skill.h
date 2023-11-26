#pragma once

#include "Client_Defines.h"
#include "Base.h"
BEGIN(Client)

class CSkill abstract : public CBase
{

private:
	CSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strSkillTag);
	CSkill(const CSkill& rhs);
	virtual ~CSkill() = default;


public:
	virtual void Tick(_float fTimeDelta);


protected:
	wstring m_strSkillTag;
	_float m_fSkillCoolTime = 1.f;
	_float m_fAccSkillCoolTime = 0.f;




public:
	virtual void Free() override;
};

END


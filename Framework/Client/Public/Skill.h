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


protected:
	wstring m_strSkillTag;
public:
	virtual void Free() override;


};

END


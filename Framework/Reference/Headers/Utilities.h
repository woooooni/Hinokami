#pragma once

#include "Base.h"

BEGIN(Engine)

class CUtilities : public CBase
{
	DECLARE_SINGLETON(CUtilities)

private:
	CUtilities();
	virtual ~CUtilities() = default;

public:
	string wstring_to_string(const wstring& strW);
	wstring string_to_wstring(const string& strS);

public:
	virtual void	Free() override;
};

END
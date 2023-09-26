#pragma once

#include "Base.h"

BEGIN(Engine)

class CUtils : public CBase
{
	DECLARE_SINGLETON(CUtils)

private:
	CUtils();
	virtual ~CUtils() = default;

public:
	string wstring_to_string(wstring strW);
	wstring string_to_wstring(string strS);

public:
	virtual void	Free();
};

END
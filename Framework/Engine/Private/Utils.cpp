#include "../Public/Utils.h"
IMPLEMENT_SINGLETON(CUtils)

CUtils::CUtils()
{

}

string CUtils::wstring_to_string(wstring strW)
{
	return string(strW.begin(), strW.end());
}

wstring CUtils::string_to_wstring(string strS)
{
	return wstring(strS.begin(), strS.end());
}

void CUtils::Free(void)
{
	__super::Free();
}

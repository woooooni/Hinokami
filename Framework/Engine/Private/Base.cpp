#include "..\Public\Base.h"

CBase::CBase()
{
	int			iData = 10;

}

unsigned long CBase::AddRef()
{
	return ++m_dwRefCnt;	
}

unsigned long CBase::Release()
{
	/* �����ϳ�. */
	if (0 == m_dwRefCnt)
	{
		Free();

		delete this;

		return 0;
	}
	else
	{
		/* �����ϳ�. */
		return m_dwRefCnt--;
	}
}

void CBase::Free()
{
}

#pragma once

#include "Engine_Defines.h"

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	/* ���۷��� ī��Ʈ�� ������Ų��. */
	/* 	unsigned long : ������Ű�� �� ������ ���۷���ī��Ʈ�� �����Ѵ�. */
	unsigned long AddRef();
	
	/* ���۷��� ī��Ʈ�� ���ҽ�Ų��. or �����Ѵ�.  */
	/* 	unsigned long : ���ҽ�Ű�� �� ������ ���۷���ī��Ʈ�� �����Ѵ�. */
	unsigned long Release();

private:
	unsigned long		m_dwRefCnt = 0;

public:
	virtual void Free();

};


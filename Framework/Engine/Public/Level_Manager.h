#pragma once

#include "Base.h"

/* Open_Level*/
/* ���� ���ӳ����� Ȱ���ϰ� �ִ� ������ü�� �ּҸ� �����Ѵ�. */
/* ���� ��ü�ÿ� ���� ������ �ı��Ѵ�. + ���� ���������� �������� �ڿ����� �����Ѵ�. */

/* �����ϰ� �ִ� ������ ƽ�� �ݺ������� ȣ�����ش�. */


BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);

public:
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);

private:
	_uint						m_iCurrentLevelIndex = { 0 };
	class CLevel*				m_pCurrentLevel = { nullptr };

public:
	virtual void Free() override;
};

END
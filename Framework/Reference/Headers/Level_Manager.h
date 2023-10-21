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
	USE_LOCK

public:
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	HRESULT Render_Debug();

public:
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);

private:
	_uint						m_iCurrentLevelIndex = { 0 };
	_uint						m_iNextLevelIndex = { 0 };

	class CLevel*				m_pCurrentLevel = { nullptr };
	class CLevel*				m_pReserveLevel = { nullptr };

public:
	virtual void Free() override;
};

END
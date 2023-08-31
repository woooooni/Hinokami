#pragma once

#include "Client_Defines.h"

/* Ŭ������ ����� ���. */
/* 
class CŬ�����̸�
{
private or protected:
	������. 
	�Ҹ���.
public: 
	Getter 
public:	 
	Setter 
public:
	�Լ�
protected:
	����
protected:
	�Լ�
private:
	����
private:
	�Լ�

public:
	���������� �Լ�. (Create, Clone)	
	���������� �Լ�. (Free)
};
*/

/* �� ������ �������� �ʱ�ȭ(Initialize), ����(Tick), ������(Render)�� �����Ѵ�. */

#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	/* �� ������ ����. */
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };

	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	HRESULT Open_Level(LEVELID eLevelID);

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END

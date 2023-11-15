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
class CRenderer;
END

BEGIN(Client)
class CNetwork_Manager;

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
	CGameInstance* m_pGame_Instance = { nullptr };
	CNetwork_Manager* m_pNetwork_Manager = { nullptr };

	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CRenderer*				m_pRenderer_Com = { nullptr };


private:
private:
	_float			m_fTimeAcc = 0.f;
	_tchar			m_szFPS[MAX_PATH] = TEXT("");
	_uint			m_iNumDraw = 0;

private:
	HRESULT Open_Level(LEVELID eLevelID, const wstring& strFolderName);

private:
	HRESULT Initialize_Client();
	HRESULT Ready_Prototype_Component();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END



#pragma once

#include "Engine_Defines.h"
#include "Base.h"

typedef struct tKeyInfo
{
	KEY_STATE	eState;
	bool		bPrevPushed;
} KEY_INFO;


BEGIN(Engine)


class CKey_Manager final : public CBase
{
	DECLARE_SINGLETON(CKey_Manager)

public:
	CKey_Manager();
	virtual ~CKey_Manager() = default;

public:
	HRESULT Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd);
	void	Tick(_float fTimeDelta);


public:
	KEY_STATE GetKeyState(KEY _eKey) { return m_vecKey[(int)_eKey].eState; }
	const POINT& GetMousePos() { return m_tMousePos; }

public:
	void Lock_Mouse() { m_bLockMouse = true; }
	void UnLock_Mouse() { m_bLockMouse = false; }
	
private:
	vector<KEY_INFO> m_vecKey;

	HWND	m_hWnd = nullptr;

	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	POINT m_tMousePos;
	_bool m_bLockMouse = false;
public:
	virtual void Free() override;
};

END

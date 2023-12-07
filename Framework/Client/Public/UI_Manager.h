#pragma once

#include "Client_Defines.h"
#include "Pool.h"
#include "Base.h"


BEGIN(Engine)
class CEffect;
END

BEGIN(Client)
class CUI_Manager : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)
	enum GAUGE_BARTYPE {LEFT_HP, LEFT_MP, RIGHT_HP, RIGHT_MP, TYPE_END};

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	HRESULT Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Tick(_float fTimeDelta);
	void	LateTick(_float fTImeDelta);


public:
	HRESULT Battle_Start();
	HRESULT Battle_End();

public:
	HRESULT	Reset_HpBar(GAUGE_BARTYPE eBarType);
	HRESULT Reserve_HpBar(GAUGE_BARTYPE eBarType, class CCharacter* pCharacter, _uint iCharacterType);
	HRESULT Reserve_HpBar(GAUGE_BARTYPE eBarType, class CMonster* pMonster, _uint iCharacterType);

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;


private:
	class CUI_Cursor* m_pCursor = nullptr;
	class CUI_GaugeBar* m_pGaugeBars[GAUGE_BARTYPE::TYPE_END];
	class CUI_BattleStart* m_pUIBattleStart = nullptr;
	class CUI_BattleEnd* m_pUIBattleEnd = nullptr;

public:
	virtual void Free() override;
};

END

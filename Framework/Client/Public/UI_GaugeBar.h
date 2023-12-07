#pragma once

#include "Client_Defines.h"
#include "UI.h"


BEGIN(Client)
class CMonster;
class CCharacter;

class CUI_GaugeBar final : public CUI
{
public:
	enum CHARACTER_TYPE { TANJIRO, ZENITSU, KYOJURO, ENMU, AKAZA, CHARACTER_END };
	enum GAUGE_TYPE { HP, MP, GAUGE_END };
	enum GAUGE_BAR_POSITION { LEFT_TOP, RIGHT_TOP, LEFT_TOP_BOTTOM, RIGHT_TOP_BOTTOM, POSITION_END };
	

protected:
	CUI_GaugeBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_GaugeBar(const CUI_GaugeBar& rhs);
	virtual ~CUI_GaugeBar() = default;

public:
	virtual HRESULT Initialize_Prototype(GAUGE_TYPE eGaugeType, GAUGE_BAR_POSITION ePosition);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	void Set_Owner(class CNpc* pOwner, CHARACTER_TYPE eType);
	void Set_Owner(class CMonster* pOwner, CHARACTER_TYPE eType);
	void Set_Owner(class CCharacter* pOwner, CHARACTER_TYPE eType);
	void Reset();

protected:
	virtual HRESULT Ready_Components() override;
	virtual void Debug_Input(_float fTimeDelta);

private:
	HRESULT Bind_ShaderResources();

private:
	GAUGE_TYPE m_eGaugeType = GAUGE_END;
	GAUGE_BAR_POSITION m_eBarPositon = POSITION_END;

	CTransform* m_pBackGroundTransform = nullptr;
	CTexture* m_pBackGroundTextureCom = nullptr;

	CTexture* m_pIconTextureCom = nullptr;
	CTransform* m_pIconTransformCom = nullptr;

	CHARACTER_TYPE m_eCharacterType = CHARACTER_TYPE::TANJIRO;

private:
	class CMonster* m_pOwnerMonster = nullptr;
	class CCharacter* m_pOwnerCharacter = nullptr;
	class CNpc* m_pOwnerNpc = nullptr;

private:
	_float m_fCurrRatio = 1.f;
	_float m_fDestRatio = 1.f;
	

public:
	static CUI_GaugeBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, GAUGE_TYPE eGaugeType, GAUGE_BAR_POSITION ePosition);
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END
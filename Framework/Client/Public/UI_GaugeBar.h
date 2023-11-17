#pragma once

#include "Client_Defines.h"
#include "UI.h"


BEGIN(Client)
class CMonster;
class CCharacter;

class CUI_GaugeBar final : public CUI
{
public:
	enum GAUGE_TYPE { HP, MP, GAUGE_END };
	enum GAUGE_BAR_POSITION { LEFT_TOP, RIGHT_TOP, LEFT_TOP_BOTTOM, RIGHT_TOP_BOTTOM, POSITION_END };
	enum CHARACTER_TYPE { TANJIRO, ZENITSU, KYOJURO,  ENMU, AKAZA, CHARACTER_END };

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
	void Set_Owner(class CNpc* pOwner, CHARACTER_TYPE eType) { 
		m_pOwnerNpc = pOwner; 
		m_eCharacterType = eType; 
	}
	void Set_Owner(class CMonster* pOwner, CHARACTER_TYPE eType) { 
		m_pOwnerMonster = pOwner; 
		m_eCharacterType = eType;
	}
	void Set_Owner(class CCharacter* pOwner, CHARACTER_TYPE eType) { 
		m_pOwnerCharacter = pOwner; 
		m_eCharacterType = eType;
	}
	

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

public:
	static CUI_GaugeBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, GAUGE_TYPE eGaugeType, GAUGE_BAR_POSITION ePosition);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END
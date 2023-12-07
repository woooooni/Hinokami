#pragma once

#include "Client_Defines.h"
#include "Level.h"
BEGIN(Engine)
class CRenderer;
END


BEGIN(Client)

class CLevel_Train_Boss final : public CLevel
{
private:
	CLevel_Train_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Train_Boss() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;

public:
	virtual HRESULT Enter_Level() override;
	virtual HRESULT Exit_Level() override;

private:
	list<LAYER_TYPE> m_ScrollObjectLayer;
	_float m_fScrollSpeed = 100.f;

	_float m_fAccScroll = 0.f;
	_float m_fLimitScroll = 290.f;

private:
	_float m_fAccSmoke = 0.f;
	_float m_fGenSmoke = 0.01f;




private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const LAYER_TYPE eLayerType);
	HRESULT Ready_Layer_Player(const LAYER_TYPE eLayerType);
	HRESULT Ready_Layer_Character(const LAYER_TYPE eLayerType);
	HRESULT Ready_Layer_BackGround(const LAYER_TYPE eLayerType);
	HRESULT Ready_Layer_Monster(const LAYER_TYPE eLayerType);
	HRESULT Ready_Layer_UI(const LAYER_TYPE eLayerType);
	HRESULT Ready_Layer_Effect(const LAYER_TYPE eLayerType);


private:
	void Scroll(_float fTimeDelta);
	void Reset_Scroll();

public:
	static class CLevel_Train_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
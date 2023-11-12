#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_NextFog final : public CUI
{
protected:
	CUI_NextFog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_NextFog(const CUI_NextFog& rhs);
	virtual ~CUI_NextFog() = default;

public:
	virtual HRESULT Initialize_Prototype(const UI_INFO& tInfo);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render(class CVIBuffer_Instancing* pBufferInstance, const vector<_float4x4>& WorldMatrices) override;

protected:
	virtual HRESULT Ready_Components() override;

private:
	HRESULT Bind_ShaderResources();

private:
	LEVELID m_eNextLevel = LEVELID::LEVEL_END;
	_float m_fAccTime = 0.f;
	_float m_fNextTime = 0.05f;
	_uint m_iTextureIndex = 0;

public:
	static CUI_NextFog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const UI_INFO& tInfo);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
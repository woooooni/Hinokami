#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CPipeLine;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CRect_Effect final : public CEffect
{

private:
	CRect_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	CRect_Effect(const CRect_Effect& rhs);
	virtual ~CRect_Effect() = default;


public:
	virtual HRESULT Initialize_Prototype(const EFFECT_DESC& EffectDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	class CVIBuffer_Rect* m_pRectBuffer = nullptr;
	class CTexture* m_pTexture = nullptr;

protected:
	virtual HRESULT Ready_Components() override;

public:
	static CRect_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const EFFECT_DESC& EffectDesc);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END


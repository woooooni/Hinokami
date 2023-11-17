#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

class CUI abstract : public CGameObject
{
public:
	enum UI_MOUSESTATE { MOUSE_ENTER, MOUSE_ON, MOUSE_EXIT, MOUSE_END };

public:
	typedef struct tagUI_Info
	{
		_float fX	= 0.f;
		_float fY	= 0.f;
		_float fCX	= 0.f;
		_float fCY	= 0.f;
	} UI_INFO;
	
protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;


public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override { return S_OK; }
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override {}
	virtual HRESULT Render() override { return S_OK; }

public:
	const UI_INFO& Get_UI_Info() { return m_tInfo; }
	void Set_UI_Info(const UI_INFO& tInfo) { m_tInfo = tInfo; }

public:
	void Set_Text(const wstring& strText) { m_strText = strText; }

protected:
	virtual void On_MouseEnter(_float fTimeDelta) {}
	virtual void On_Mouse(_float fTimeDelta) {}
	virtual void On_MouseExit(_float fTimeDelta) {}


protected:
	virtual void Debug_Input(_float fTimeDelta);

protected:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

protected:
	UI_INFO m_tInfo = { 0 };
	UI_MOUSESTATE m_eMouseState = UI_MOUSESTATE::MOUSE_END;

	_float4x4 m_ViewMatrix;
	_float4x4 m_ProjMatrix;

	wstring m_strText = L"";

public:
	virtual void Free() override;
};


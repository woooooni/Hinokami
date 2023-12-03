#pragma once

#include "Client_Defines.h"
#include "Part.h"


BEGIN(Client)

class CSweath final : public CPart
{
public:
	enum SWEATH_TYPE { TANJIRO, ZENITSU, KYOJURO, SWEATH_END };

public:
	typedef struct tagWeaponDesc : public CPart::PART_DESC
	{		
		_float3			vRotationDegree;
		SWEATH_TYPE eType;
	} SWEATH_DESC;

protected:
	/* 원형을 생성할 때 */
	CSweath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	/* 사본을 생성할 때 */
	CSweath(const CSweath& rhs); /* 복사 생성자. */

	virtual ~CSweath() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strPrototypeModelName);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();



private:
	wstring					m_strModelPrototype;
	SWEATH_TYPE				m_eType = SWEATH_END;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSweath* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const wstring& strPrototypeModelName);
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END
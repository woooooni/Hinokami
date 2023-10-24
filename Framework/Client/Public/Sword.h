#pragma once

#include "Client_Defines.h"
#include "Part.h"


BEGIN(Client)

class CSword final : public CPart
{
public:
	typedef struct tagWeaponDesc : public CPart::PART_DESC
	{		
		_float3			vRotationDegree;
	} SWORD_DESC;

protected:
	/* 원형을 생성할 때 */
	CSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	/* 사본을 생성할 때 */
	CSword(const CSword& rhs); /* 복사 생성자. */

	virtual ~CSword() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strPrototypeSwordModel);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	void Generate_Trail();
	void Stop_Trail();
	void Generate_Effect();

private:
	wstring					m_strModelPrototype;

private:
	HRESULT Ready_Components();
	HRESULT Ready_Colliders();

	HRESULT Bind_ShaderResources();

public:
	static CSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const wstring& strPrototypeSwordModel);
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END
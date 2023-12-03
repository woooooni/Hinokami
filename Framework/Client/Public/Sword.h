#pragma once

#include "Client_Defines.h"
#include "Part.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CSword final : public CPart
{
public:
	enum SWORD_TYPE { TANJIRO, ZENITSU, KYOJURO, SWORD_END };
public:
	typedef struct tagWeaponDesc : public CPart::PART_DESC
	{		
		_float3			vRotationDegree;
		SWORD_TYPE eType;
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
	virtual void Collision_Enter(const COLLISION_INFO& tInfo) override;
	virtual void Collision_Continue(const COLLISION_INFO& tInfo) override;
	virtual void Collision_Exit(const COLLISION_INFO& tInfo) override;


public:
	SWORD_TYPE Get_SwordType() { return m_eType; }
	_matrix Get_FinalWorldMatrix();
		

public:
	void Set_Sweath(_bool bSweath) { m_bSweath = bSweath; }
	void SetUp_Trail_Position(_vector vHighPosition, _vector vLowPosition);
	void Generate_Trail(const wstring& strDiffuseTextureName, const wstring& strAlphaTextureName, const _float4& vColor, _uint iVertexCount);
	void Stop_Trail();

private:
	wstring					m_strModelPrototype;
	_float					m_fDamage = 1.f;
	SWORD_TYPE				m_eType = SWORD_END;
	_bool					m_bSweath = true;

private:
	class CTrail* m_pTrailObject = { nullptr };
	



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
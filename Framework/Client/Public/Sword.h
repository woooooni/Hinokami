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
	/* ������ ������ �� */
	CSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	/* �纻�� ������ �� */
	CSword(const CSword& rhs); /* ���� ������. */

	virtual ~CSword() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strPrototypeSwordModel);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render(class CVIBuffer_Instancing* pBufferInstance, const vector<_float4x4>& WorldMatrices) override;

public:
	void Set_Damage(_float fDamage) { m_fDamage = fDamage; }
	_float Get_Damage() { return m_fDamage; }

public:
	virtual void Collision_Enter(const COLLISION_INFO& tInfo) override;
	virtual void Collision_Continue(const COLLISION_INFO& tInfo) override;
	virtual void Collision_Exit(const COLLISION_INFO& tInfo) override;


public:
	_matrix Get_FinalWorldMatrix();
		

public:
	void Generate_Trail(const wstring& strDiffuseTextureName, const wstring& strAlphaTextureName, const _float4& vColor, _uint iVertexCount);
	void Stop_Trail();

private:
	wstring					m_strModelPrototype;
	_float					m_fDamage = 1.f;
	SWORD_TYPE				m_eType = SWORD_END;

private:
	class CTrail* m_pTrailObject = { nullptr };
	



private:
	HRESULT Ready_Components();
	HRESULT Ready_Colliders();

	HRESULT Bind_ShaderResources();

public:
	static CSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const wstring& strPrototypeSwordModel);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END
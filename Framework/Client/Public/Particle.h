#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CPipeLine;
class CTransform;
class CRigidBody;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CEffect;

class CParticle final : public CGameObject
{
public:
	typedef struct tagPaticleDesc
	{	
		// Initialize
		_uint iNumEffectCount = 0;
		
		_float2		vRandomSpeed;
		_float3		vRandomDir;


		// RigidBody.
		_bool bGravity = false;
		_float fForceMax = 10.f;
		_float3 vRandomForce = { 0.f, 0.f, 0.f };
		


		_float2		vEffectRandomLifeTime = { 0.f, 0.f };
		_float		fLifeTime = 0.f;

	} PARTICLE_DESC;

protected:
	CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const wstring& strEffectName);
	CParticle(const CParticle& rhs);
	virtual ~CParticle() = default;


public:
	virtual HRESULT Initialize_Prototype(const PARTICLE_DESC& tEffectDesc);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;



public:
	const PARTICLE_DESC& Get_ParticleDesc() { return m_tParticleDesc; }
	void Set_ParticleDesc(const PARTICLE_DESC& tDesc) { memcpy(&m_tParticleDesc, &tDesc, sizeof(PARTICLE_DESC)); }

	void Set_Gravity(_bool bGravity);
	const wstring& Get_EffectPrototypeName() { return m_strPrototypeEffectTag; }
 
public:
	HRESULT Ready_Effects();
	class CEffect* Generate_Effect();




public:
	class CTransform* Get_TransformCom() { return m_pTransformCom; }
	

private:
	list<class CEffect*> m_Effects;
	vector<_float> m_EffectLifeTimes;



	class CTransform* m_pTransformCom = nullptr;

private:
	PARTICLE_DESC m_tParticleDesc;


	_float m_fAccLifeTime = 0.f;
	wstring m_strPrototypeEffectTag;

private:
	HRESULT Bind_ShaderResource();

protected:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT Ready_Components() override;

public:
	static CParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
		const wstring& strObjectTag, const wstring& strPrototypeEffectTag, const PARTICLE_DESC& ParticleDesc);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END



#include "stdafx.h"
#include "Particle.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect.h"
#include "Utils.h"



CParticle::CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const wstring& strEffectName)
	: CGameObject(pDevice, pContext, strObjectTag, OBJ_TYPE::OBJ_PARTICLE)
	, m_strPrototypeEffectTag(strEffectName)
{

}

CParticle::CParticle(const CParticle& rhs)
	: CGameObject(rhs)
	, m_tParticleDesc(rhs.m_tParticleDesc)
	, m_fAccLifeTime(0.f)
	, m_strPrototypeEffectTag(rhs.m_strPrototypeEffectTag)
{
	
}

HRESULT CParticle::Initialize_Prototype(const PARTICLE_DESC& ParticleDesc)
{
	m_tParticleDesc = ParticleDesc;

	if (nullptr == GI->Find_Prototype_GameObject(LAYER_EFFECT, m_strPrototypeEffectTag))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CParticle::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Effects()))
		return E_FAIL;
	
	m_fAccLifeTime = 0.f;

	return S_OK;
}

void CParticle::Tick(_float fTimeDelta)
{
	if (!m_bDead)
	{
		m_fAccLifeTime += fTimeDelta;
		if (m_tParticleDesc.fLifeTime <= m_fAccLifeTime)
		{
			Set_Dead(true);
			return;
		}
	}
	else
		return;


	for (auto& pEffect : m_Effects)
	{
		pEffect->Set_ParentMatrix(m_pTransformCom->Get_WorldMatrix());
		pEffect->Tick(fTimeDelta);
	}
}

void CParticle::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	for (auto& pEffect : m_Effects)
		pEffect->LateTick(fTimeDelta);
}

HRESULT CParticle::Render()
{

	return S_OK;
}

void CParticle::Set_Gravity(_bool bGravity)
{
	for (auto& pEffect : m_Effects)
		pEffect->Set_Gravity(bGravity);
}

HRESULT CParticle::Ready_Effects()
{
	for (auto& pEffect : m_Effects)
		Safe_Release(pEffect);

	m_Effects.clear();

	m_fAccLifeTime = 0.f;

	for (_uint i = 0; i < m_tParticleDesc.iNumEffectCount; ++i)
	{
		CEffect* pEffect = Generate_Effect();
		if (pEffect == nullptr)
			return E_FAIL;

		Safe_AddRef(pEffect);
		m_Effects.push_back(pEffect);
	}
	return S_OK;
}

Client::CEffect* CParticle::Generate_Effect()
{
	CGameObject* pGameObject = GI->Clone_GameObject(m_strPrototypeEffectTag, LAYER_EFFECT);
	if (nullptr == pGameObject)
		return nullptr;

	CEffect* pEffect = dynamic_cast<CEffect*>(pGameObject);
	if (nullptr == pEffect)
		return nullptr;

	CEffect::EFFECT_DESC EffectDesc = pEffect->Get_EffectDesc();
	EffectDesc.bBillboard = m_tParticleDesc.bBillboard;
	

	_float fSpeed = m_tParticleDesc.fSpeed;
	if (m_tParticleDesc.bRandomSpeed)
	{
		fSpeed = CUtils::Random_Float(1.f, fSpeed);
	}
	EffectDesc.fMoveSpeed = fSpeed;



	_float3 vDir = m_tParticleDesc.vDir;
	if (m_tParticleDesc.bRandomDir)
	{
		vDir.x = CUtils::Random_Float(-vDir.x, vDir.x);
		vDir.y = CUtils::Random_Float(0.f, vDir.y);
		vDir.z = CUtils::Random_Float(-vDir.z, vDir.z);
	}
	XMStoreFloat3(&EffectDesc.vMoveDir, XMVector3Normalize(XMLoadFloat3(&vDir)));

	pEffect->Set_Gravity(m_tParticleDesc.bRigidActive);
	_float3 vForceDir = m_tParticleDesc.vForceDir;
	if (m_tParticleDesc.bRandomForceDir)
	{
		vForceDir.x = CUtils::Random_Float(-vForceDir.x, vForceDir.x);
		vForceDir.y = CUtils::Random_Float(-vForceDir.y, vForceDir.y);
		vForceDir.z = CUtils::Random_Float(-vForceDir.z, vForceDir.z);
	}

	_float fFrictionScale = m_tParticleDesc.fFrictionScale;
	if (m_tParticleDesc.bRandomFriction)
	{
		fFrictionScale = CUtils::Random_Float(0.f, fFrictionScale);
	}
	pEffect->Get_RigidBodyCom()->Set_Friction_Scale(fFrictionScale);
	pEffect->Get_RigidBodyCom()->Add_Velocity(XMVector3Normalize(XMLoadFloat3(&vForceDir)), m_tParticleDesc.fForce);

	EffectDesc.fBlurPower = m_tParticleDesc.fBlurPower;
	EffectDesc.fDestAlphaSpeed = m_tParticleDesc.fDestAlphaSpeed;
	
	pEffect->Set_EffectDesc(EffectDesc);

	pEffect->Set_DeletionTime(m_tParticleDesc.fLifeTime);
	
	return pEffect;
}



HRESULT CParticle::Bind_ShaderResource()
{
	return S_OK;
}



HRESULT CParticle::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	

	return S_OK;
}


CParticle* CParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, const wstring& strEffectName, const PARTICLE_DESC& ParticleDesc)
{
	CParticle* pInstance = new CParticle(pDevice, pContext, strObjectTag, strEffectName);
	if (FAILED(pInstance->Initialize_Prototype(ParticleDesc)))
	{
		MSG_BOX("Failed to Created : CParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CParticle::Clone(void* pArg)
{
	CParticle* pInstance = new CParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle::Free()
{
	__super::Free();

	for (auto& pEffect : m_Effects)
	{
		Safe_Release(pEffect);
	}
	m_Effects.clear();
	Safe_Release(m_pTransformCom);
}






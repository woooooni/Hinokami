#include "stdafx.h"
#include "Particle.h"
#include "GameInstance.h"

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
	

	return S_OK;
}

void CParticle::Tick(_float fTimeDelta)
{
	if (!m_bDead)
	{
		m_fAccLifeTime += fTimeDelta;
		if (m_tParticleDesc.fLifeTime <= m_fAccLifeTime)
		{
			m_fAccLifeTime = 0.f;

			for (auto& pEffect : m_Effects)
				Safe_Release(pEffect);

			m_EffectLifeTimes.clear();
			m_Effects.clear();

			Set_Dead(true);
			return;
		}
	}
	else
	{
		return;
	}


	while(m_tParticleDesc.iNumEffectCount > m_Effects.size())
	{
		CEffect* pEffect = Generate_Effect();
		if (nullptr == pEffect)
		{
			MSG_BOX("Effect Generate Failed. :CParticle");
			break;
		}
		m_Effects.push_back(pEffect);
		m_EffectLifeTimes.push_back(CUtils::Random_Float(m_tParticleDesc.vEffectRandomLifeTime.x, m_tParticleDesc.vEffectRandomLifeTime.y));
	}

	_uint iEffectIndex = 0;
	auto iter = m_Effects.begin();
	
	while (iter != m_Effects.end())
	{	
		if (m_tParticleDesc.iNumEffectCount < m_Effects.size())
		{
			Safe_Release((*iter));
			auto timeIter = m_EffectLifeTimes.begin() + iEffectIndex;
			
			iter = m_Effects.erase(iter);
			m_EffectLifeTimes.erase(timeIter);
			continue;
		}

		if (m_EffectLifeTimes[iEffectIndex] <= m_fAccLifeTime)
		{
			Safe_Release((*iter));

			auto timeIter = m_EffectLifeTimes.begin() + iEffectIndex;

			iter = m_Effects.erase(iter);
			m_EffectLifeTimes.erase(timeIter);

			continue;
		}
		
		(*iter)->Set_ParentMatrix(m_pTransformCom->Get_WorldMatrix());

		(*iter)->Tick(fTimeDelta);
		++iter;
		++iEffectIndex;
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
	__super::Render();

	for (auto& pEffect : m_Effects)
		pEffect->Render();

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
	m_EffectLifeTimes.clear();

	for (_uint i = 0; i < m_tParticleDesc.iNumEffectCount; ++i)
	{
		CGameObject* pObject = GI->Clone_GameObject(m_strPrototypeEffectTag, LAYER_EFFECT);
		if (nullptr == pObject)
			return E_FAIL;

		CEffect* pEffect = dynamic_cast<CEffect*>(pObject);
		if (nullptr == pEffect)
			return E_FAIL;

		pEffect->Set_Gravity(m_tParticleDesc.bGravity);
		if (m_tParticleDesc.bGravity)
		{
			_float3 vRandomForce =
				_float3(CUtils::Random_Float(m_tParticleDesc.vRandomForce.x, m_tParticleDesc.fForceMax),
					CUtils::Random_Float(m_tParticleDesc.vRandomForce.y, m_tParticleDesc.fForceMax),
					CUtils::Random_Float(m_tParticleDesc.vRandomForce.z, m_tParticleDesc.fForceMax));

			CTransform* pTransform = pEffect->Get_Component<CTransform>(L"Com_Transform");
			CRigidBody* pRigidBody = pEffect->Get_Component<CRigidBody>(L"Com_RigidBody");

			if (m_tParticleDesc.vRandomForce.x == 0.f)
				vRandomForce.x = 0.f;

			if (m_tParticleDesc.vRandomForce.y == 0.f)
				vRandomForce.y = 0.f;

			if (m_tParticleDesc.vRandomForce.z == 0.f)
				vRandomForce.z = 0.f;

			_vector vPosition = pTransform->Get_State(CTransform::STATE_POSITION);
			XMVectorSetY(vPosition, XMVectorGetY(vPosition) + 0.01f);
			pTransform->Set_State(CTransform::STATE_POSITION, vPosition);

			pRigidBody->Add_Velocity(XMLoadFloat3(&vRandomForce));
		}

		_vector vRandomDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		if (XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_tParticleDesc.vRandomDir))) <= 0.f)
		{
			XMVectorSetY(vRandomDir, 1.f);
		}

		while (0.01f > XMVectorGetX(XMVector3Length(vRandomDir)))
		{
			vRandomDir = XMVectorSet(
				CUtils::Random_Float(-m_tParticleDesc.vRandomDir.x, m_tParticleDesc.vRandomDir.x),
				CUtils::Random_Float(-m_tParticleDesc.vRandomDir.y, m_tParticleDesc.vRandomDir.y),
				CUtils::Random_Float(-m_tParticleDesc.vRandomDir.z, m_tParticleDesc.vRandomDir.z), 0.f);
		}
		pEffect->Set_MoveDir(XMVector3Normalize(vRandomDir));

		_float fRandomSpeed = CUtils::Random_Float(m_tParticleDesc.vRandomSpeed.x, m_tParticleDesc.vRandomSpeed.y);
		pEffect->Set_MoveSpeed(fRandomSpeed);


		Safe_AddRef(pEffect);
		m_Effects.push_back(pEffect);
		m_EffectLifeTimes.push_back(CUtils::Random_Float(m_tParticleDesc.vEffectRandomLifeTime.x, m_tParticleDesc.vEffectRandomLifeTime.y));
	}
	return S_OK;
}

CEffect* CParticle::Generate_Effect()
{
	CGameObject* pObject = GI->Clone_GameObject(m_strPrototypeEffectTag, LAYER_EFFECT);
	if (nullptr == pObject)
		return nullptr;

	CEffect* pEffect = dynamic_cast<CEffect*>(pObject);
	if (nullptr == pEffect)
		return nullptr;

	pEffect->Set_Gravity(m_tParticleDesc.bGravity);
	if (m_tParticleDesc.bGravity)
	{
		_float3 vRandomForce =
			_float3(CUtils::Random_Float(m_tParticleDesc.vRandomForce.x, m_tParticleDesc.fForceMax),
				CUtils::Random_Float(m_tParticleDesc.vRandomForce.y, m_tParticleDesc.fForceMax),
				CUtils::Random_Float(m_tParticleDesc.vRandomForce.z, m_tParticleDesc.fForceMax));

		CTransform* pTransform = pEffect->Get_Component<CTransform>(L"Com_Transform");
		CRigidBody* pRigidBody = pEffect->Get_Component<CRigidBody>(L"Com_RigidBody");

		if (m_tParticleDesc.vRandomForce.x == 0.f)
			vRandomForce.x = 0.f;

		if (m_tParticleDesc.vRandomForce.y == 0.f)
			vRandomForce.y = 0.f;

		if (m_tParticleDesc.vRandomForce.z == 0.f)
			vRandomForce.z = 0.f;

		_vector vPosition = pTransform->Get_State(CTransform::STATE_POSITION);
		XMVectorSetY(vPosition, XMVectorGetY(vPosition) + 0.01f);
		pTransform->Set_State(CTransform::STATE_POSITION, vPosition);

		pRigidBody->Add_Velocity(XMLoadFloat3(&vRandomForce));
	}

	_vector vRandomDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_tParticleDesc.vRandomDir))) <= 0.f)
	{
		XMVectorSetY(vRandomDir, 1.f);
	}

	while (0.01f > XMVectorGetX(XMVector3Length(vRandomDir)))
	{
		vRandomDir = XMVectorSet(
			CUtils::Random_Float(-m_tParticleDesc.vRandomDir.x, m_tParticleDesc.vRandomDir.x),
			CUtils::Random_Float(-m_tParticleDesc.vRandomDir.y, m_tParticleDesc.vRandomDir.y),
			CUtils::Random_Float(-m_tParticleDesc.vRandomDir.z, m_tParticleDesc.vRandomDir.z), 0.f);
	}
	pEffect->Set_MoveDir(XMVector3Normalize(vRandomDir));

	_float fRandomSpeed = CUtils::Random_Float(m_tParticleDesc.vRandomSpeed.x, m_tParticleDesc.vRandomSpeed.y);
	pEffect->Set_MoveSpeed(fRandomSpeed);

	Safe_AddRef(pEffect);
	
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
		Safe_Release(pEffect);

	m_Effects.clear();
	m_EffectLifeTimes.clear();

}






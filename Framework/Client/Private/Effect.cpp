#include "stdafx.h"
#include "Effect.h"
#include "GameInstance.h"



CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, 
	EFFECT_TYPE eType, const wstring& strPrototypeModelName, _bool bIncrement,
	_bool bLoop, _bool bGravity)
	: CGameObject(pDevice, pContext, strObjectTag, OBJ_TYPE::OBJ_EFFECT)
	, m_eType(eType)
	, m_strModelPrototype(strPrototypeModelName)
	, m_bIncrement(bIncrement)
	, m_bLoop(bLoop)
	, m_bGravity(bGravity)
{

}

CEffect::CEffect(const CEffect& rhs)
	: CGameObject(rhs)
	, m_eType(rhs.m_eType)
	, m_tEffectDesc(rhs.m_tEffectDesc)
	, m_strModelPrototype(rhs.m_strModelPrototype)
	, m_iPassIndex(rhs.m_iPassIndex)
	, m_bIncrement(rhs.m_bIncrement)
	, m_bLoop(rhs.m_bLoop)
	, m_bGravity(rhs.m_bGravity)
	, m_fAccUVFlow(rhs.m_fAccUVFlow)
	, m_fAccDeletionTime(0.f)
{

}

HRESULT CEffect::Initialize_Prototype(const EFFECT_DESC& tEffectDesc)
{
	m_tEffectDesc = tEffectDesc;
	return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pRigidBodyCom->Set_RefHeight(0.f);
	m_pRigidBodyCom->Set_Gravity(m_bGravity);

	XMStoreFloat4x4(&m_ParentMatrix, XMMatrixIdentity());

	return S_OK;
}

void CEffect::Tick(_float fTimeDelta)
{
	m_fAccDeletionTime += fTimeDelta;

	if (m_fAccDeletionTime >= m_fDeletionTime)
	{
		m_fAccDeletionTime = 0.f;
		Set_Dead(true);
	}

	if (m_bIncrement)
		Increment(fTimeDelta);
	else
		Decrement(fTimeDelta);

	m_fAccUVFlow.x += m_tEffectDesc.fUVFlow.x * fTimeDelta;
	m_fAccUVFlow.y += m_tEffectDesc.fUVFlow.y * fTimeDelta;


	m_pRigidBodyCom->Tick_RigidBody(fTimeDelta);
	
	_vector vMoveDir = XMVector3Normalize(XMLoadFloat3(&m_tEffectDesc.vMoveDir));
	_vector vTurnDir = XMVector3Normalize(XMLoadFloat3(&m_tEffectDesc.vTurnDir));

	m_pTransformCom->Go_Dir(vMoveDir, m_tEffectDesc.fMoveSpeed, fTimeDelta);
	m_pTransformCom->Turn(vTurnDir, m_tEffectDesc.fTurnSpeed, fTimeDelta);
}

void CEffect::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffect::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResource()))
		return E_FAIL;

	if (m_eType == EFFECT_TYPE::EFFECT_MESH)
	{
		_uint iNumMesh = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMesh; ++i)
			m_pModelCom->Render(m_pShaderCom, i, m_iPassIndex);
	}
	else
	{
		m_pShaderCom->Begin(m_iPassIndex);
		m_pVIBufferCom->Render();
	}

	return S_OK;
}

HRESULT CEffect::Save_EffectInfo()
{
	return S_OK;
}

void CEffect::Set_MoveDir(_vector vDir)
{
	XMStoreFloat3(&m_tEffectDesc.vMoveDir, XMVector3Normalize(vDir));
}

void CEffect::Increment(_float fTimeDelta)
{
	if (m_bEnd == true && m_bLoop == false)
		return;

	m_fAccIndex += m_tEffectDesc.fIndexSpeed * fTimeDelta;
	if (m_fAccIndex >= 1.f)
	{
		m_fAccIndex = 0.f;
		m_vUVIndex.x++;
		if (m_tEffectDesc.fMaxCountX <= m_vUVIndex.x)
		{
			m_vUVIndex.x = 0;
			m_vUVIndex.y++;
			if (m_tEffectDesc.fMaxCountY <= m_vUVIndex.y)
			{
				m_vUVIndex.y = 0;
				if (m_bLoop == false)
					m_bEnd = true;
				else
					m_bEnd = false;
			}
		}
	}
}

void CEffect::Decrement(_float fTimeDelta)
{
	if (m_bEnd == true && m_bLoop == false)
		return;




	m_fAccIndex += m_tEffectDesc.fIndexSpeed * fTimeDelta;

	if (m_fAccIndex >= 1.f)
	{
		m_fAccIndex = 0.f;
		m_vUVIndex.x--;
		if (0 > m_vUVIndex.x)
		{
			m_vUVIndex.x = m_tEffectDesc.fMaxCountX;
			m_vUVIndex.y--;
			if (0 > m_vUVIndex.y)
			{
				m_vUVIndex.y = m_tEffectDesc.fMaxCountY;
				if (m_bLoop == false)
					m_bEnd = true;
				else
					m_bEnd = false;
			}
		}
	}
}

HRESULT CEffect::Bind_ShaderResource()
{
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaxCountX", &m_tEffectDesc.fMaxCountX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaxCountY", &m_tEffectDesc.fMaxCountY, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_tEffectDesc.fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVIndex", &m_vUVIndex, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAdditiveDiffuseColor", &m_tEffectDesc.fAdditiveDiffuseColor, sizeof(_float3))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVFlow", &m_fAccUVFlow, sizeof(_float2))))
		return E_FAIL;
	

	{

		if (-1 < m_tEffectDesc.iDiffuseTextureIndex)
		{
			if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_tEffectDesc.iDiffuseTextureIndex)))
				return E_FAIL;
		}


		if (-1 < m_tEffectDesc.iAlphaTextureIndex)
		{
			if (FAILED(m_pAlphaTextureCom->Bind_ShaderResource(m_pShaderCom, "g_AlphaTexture", m_tEffectDesc.iAlphaTextureIndex)))
				return E_FAIL;
		}


		// 둘다 없다면.
		if (-1 == m_tEffectDesc.iDiffuseTextureIndex && -1 == m_tEffectDesc.iAlphaTextureIndex)
			m_iPassIndex = 0;

		// 디퓨즈 텍스쳐만 있다면.
		if (-1 != m_tEffectDesc.iDiffuseTextureIndex && -1 == m_tEffectDesc.iAlphaTextureIndex)
			m_iPassIndex = 1;

		// 알파 텍스쳐만 있다면.
		if (-1 == m_tEffectDesc.iDiffuseTextureIndex && -1 != m_tEffectDesc.iAlphaTextureIndex)
			m_iPassIndex = 2;

		if (-1 != m_tEffectDesc.iDiffuseTextureIndex && -1 != m_tEffectDesc.iAlphaTextureIndex)		
			m_iPassIndex = 3;
	}

	_float4x4 WorldMatrix;

	_matrix LocalMatrix = XMLoadFloat4x4(&m_tEffectDesc.OffsetMatrix) * m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&m_ParentMatrix);

	
	if (m_eType == EFFECT_TYPE::EFFECT_MESH)
	{
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(LocalMatrix));
	}
	else
	{
		// 빌보드를 적용한다.
		_vector vPosition = LocalMatrix.r[CTransform::STATE_POSITION];
		_vector vCamPosition = XMLoadFloat4(&GI->Get_CamPosition());

		_float fLookScale = XMVectorGetX(XMVector3Length(LocalMatrix.r[CTransform::STATE_LOOK]));
		_vector vLook = XMVectorSetW(XMVector3Normalize(vPosition - vCamPosition), 0.f) * fLookScale;

		_float fRightScale = XMVectorGetX(XMVector3Length(LocalMatrix.r[CTransform::STATE_RIGHT]));
		_vector vRight = XMVectorSetW(XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)), 0.f) * fRightScale;

		_float fUpScale = XMVectorGetX(XMVector3Length(LocalMatrix.r[CTransform::STATE_UP]));
		_vector vUp = XMVectorSetW(XMVector3Normalize(XMVector3Cross(vLook, vRight)), 0.f) * fUpScale;

		
		
		
		LocalMatrix.r[CTransform::STATE_RIGHT] = vRight;
		LocalMatrix.r[CTransform::STATE_UP] = vUp;
		LocalMatrix.r[CTransform::STATE_LOOK] = vLook;


		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(LocalMatrix));
	}
	

	
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	return S_OK;
}



HRESULT CEffect::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"), TEXT("Com_Diffuse_Texture"), (CComponent**)&m_pDiffuseTextureCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"), TEXT("Com_Alpha_Texture"), (CComponent**)&m_pAlphaTextureCom)))
		return E_FAIL;



	CRigidBody::RIGID_BODY_DESC RigidDesc;
	ZeroMemory(&RigidDesc, sizeof RigidDesc);


	RigidDesc.pNavigation = nullptr;
	RigidDesc.pTransform = m_pTransformCom;


	/* For. Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigidBodyCom, &RigidDesc)))
		return E_FAIL;

	if(m_eType == EFFECT_TYPE::EFFECT_MESH)
	{
		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Mesh_Effect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;


		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strModelPrototype, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	else
	{
		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Texture_Effect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;


		/* For.Com_VIBuffer */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
			return E_FAIL;
	}
	

	return S_OK;
}


CEffect* CEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, 
	EFFECT_TYPE eType, const wstring& strPrototypeModelName, const EFFECT_DESC& tEffectDesc, _bool bIncrement, _bool bLoop, _bool bGravity)
{
	CEffect* pInstance = new CEffect(pDevice, pContext, strObjectTag, eType, strPrototypeModelName, bIncrement, bLoop, bGravity);
	if (FAILED(pInstance->Initialize_Prototype(tEffectDesc)))
	{
		MSG_BOX("Failed to Created : CEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect::Clone(void* pArg)
{
	CEffect* pInstance = new CEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pAlphaTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pModelCom);
}






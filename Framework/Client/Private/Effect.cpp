#include "stdafx.h"
#include "Effect.h"
#include "GameInstance.h"
#include "Effect_Manager.h"



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
	, m_vUVIndex({0.f, 0.f})
	, m_fAccUVFlow({ 0.f, 0.f })
	, m_fAccDeletionTime(0.f)
	, m_iDiffuseTextureIdx(rhs.m_iDiffuseTextureIdx)
	, m_iAlphaTextureIdx(rhs.m_iAlphaTextureIdx)
	, m_fDeletionTime(rhs.m_fDeletionTime)
	, m_fAccIndex(0.f)
{
	m_tEffectDesc.fAlpha = 1.f;
}

HRESULT CEffect::Initialize_Prototype(const EFFECT_DESC& tEffectDesc)
{
	m_tEffectDesc = tEffectDesc;
	m_tOriginEffectDesc = tEffectDesc;
	return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (nullptr != pArg)
		m_pOwnerObject = (CGameObject*)pArg;

	m_iDiffuseTextureIdx = m_pDiffuseTextureCom->Find_Index(m_tEffectDesc.strDiffuseTetextureName);
	m_iAlphaTextureIdx = m_pAlphaTextureCom->Find_Index(m_tEffectDesc.strAlphaTexturName);
	XMStoreFloat4x4(&m_ParentMatrix, XMMatrixIdentity());

	return S_OK;
}

void CEffect::Tick(_float fTimeDelta)
{
	if (m_bDead && GI->Get_CurrentLevel() != LEVEL_TOOL)
		return;

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

	m_fAccUVFlow.x += m_tEffectDesc.vUVFlow.x * fTimeDelta;
	m_fAccUVFlow.y += m_tEffectDesc.vUVFlow.y * fTimeDelta;
	if(m_tEffectDesc.fAlpha > 0.f)
		m_tEffectDesc.fAlpha -= m_tEffectDesc.fDestAlphaSpeed * fTimeDelta;


	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[CTransform::STATE_RIGHT] += XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Normalize(XMLoadFloat3(&m_tEffectDesc.vScaleDir))) * m_tEffectDesc.fScaleSpeed * fTimeDelta;
	WorldMatrix.r[CTransform::STATE_UP] += XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetY(XMVector3Normalize(XMLoadFloat3(&m_tEffectDesc.vScaleDir))) * m_tEffectDesc.fScaleSpeed * fTimeDelta;
	WorldMatrix.r[CTransform::STATE_LOOK] += XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetZ(XMVector3Normalize(XMLoadFloat3(&m_tEffectDesc.vScaleDir))) * m_tEffectDesc.fScaleSpeed * fTimeDelta;
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	m_strPrototypeTag;
	
	_vector vMoveDir = XMVector3Normalize(XMLoadFloat3(&m_tEffectDesc.vMoveDir));
	_vector vTurnDir = XMVector3Normalize(XMLoadFloat3(&m_tEffectDesc.vTurnDir));

	m_pTransformCom->Go_Dir(vMoveDir, m_tEffectDesc.fMoveSpeed, fTimeDelta);
	m_pTransformCom->Turn(vTurnDir, m_tEffectDesc.fTurnSpeed, fTimeDelta);

	if (nullptr != m_pOwnerObject)
	{
		CTransform* pOwnerTransform = m_pOwnerObject->Get_Component<CTransform>(L"Com_Transform");

		if (nullptr != pOwnerTransform)
		{
			XMStoreFloat4x4(&m_ParentMatrix, pOwnerTransform->Get_WorldMatrix());
		}
	}
}

void CEffect::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	if (m_bDead && GI->Get_CurrentLevel() != LEVEL_TOOL)
		return;

	_float4x4 WolrdMatrix;
	XMStoreFloat4x4(&WolrdMatrix, XMLoadFloat4x4(&m_tEffectDesc.OffsetMatrix) * m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&m_ParentMatrix));

	if (m_tEffectDesc.bBillboard)
	{
		_matrix Temp = XMLoadFloat4x4(&WolrdMatrix);
		_vector vPosition = Temp.r[CTransform::STATE_POSITION];
		_vector vCamPosition = XMLoadFloat4(&GI->Get_CamPosition());

		_float fLookScale = XMVectorGetX(XMVector3Length(Temp.r[CTransform::STATE_LOOK]));
		_vector vLook = XMVectorSetW(XMVector3Normalize(vPosition - vCamPosition), 0.f) * fLookScale;

		_float fRightScale = XMVectorGetX(XMVector3Length(Temp.r[CTransform::STATE_RIGHT]));
		_vector vRight = XMVectorSetW(XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)), 0.f) * fRightScale;

		_float fUpScale = XMVectorGetX(XMVector3Length(Temp.r[CTransform::STATE_UP]));
		_vector vUp = XMVectorSetW(XMVector3Normalize(XMVector3Cross(vLook, vRight)), 0.f) * fUpScale;

		Temp.r[CTransform::STATE_RIGHT] = vRight;
		Temp.r[CTransform::STATE_UP] = vUp;
		Temp.r[CTransform::STATE_LOOK] = vLook;

		XMStoreFloat4x4(&WolrdMatrix, Temp);
	}
	


	m_pRigidBodyCom->LateTick_RigidBody(fTimeDelta);

	CRenderer::EFFECT_INSTANCE_DESC EffectInstanceDesc;
	ZeroMemory(&EffectInstanceDesc, sizeof CRenderer::EFFECT_INSTANCE_DESC);

	EffectInstanceDesc.g_fMaxCountX = m_tEffectDesc.fMaxCountX;
	EffectInstanceDesc.g_fMaxCountY = m_tEffectDesc.fMaxCountY;
	EffectInstanceDesc.g_fAlpha = m_tEffectDesc.fAlpha;
	EffectInstanceDesc.g_fUVIndex = m_vUVIndex;
	EffectInstanceDesc.g_fUVFlow = m_fAccUVFlow;

	EffectInstanceDesc.g_fAdditiveDiffuseColor = m_tEffectDesc.vAdditiveDiffuseColor;
	EffectInstanceDesc.g_vBloomPower = _float4(m_tEffectDesc.vBloomPower.x, m_tEffectDesc.vBloomPower.y, m_tEffectDesc.vBloomPower.z, 0.f);
	EffectInstanceDesc.g_iCutUV = m_tEffectDesc.bCutUV;



	if (true == GI->Intersect_Frustum_World(XMLoadFloat4x4(&WolrdMatrix).r[CTransform::STATE_POSITION], 3.f))
	{
		if (m_eType == EFFECT_TYPE::EFFECT_MESH)
			m_pRendererCom->Add_RenderGroup_Instancing_Effect(CRenderer::RENDER_EFFECT, CRenderer::SHADER_TYPE::EFFECT_MODEL, this, WolrdMatrix, EffectInstanceDesc);
		else
			m_pRendererCom->Add_RenderGroup_Instancing_Effect(CRenderer::RENDER_EFFECT, CRenderer::SHADER_TYPE::EFFECT_TEXTURE, this, WolrdMatrix, EffectInstanceDesc);
	}
	
}


HRESULT CEffect::Render_Instance(CShader* pInstancingShader, CVIBuffer_Instancing* pInstancingBuffer, const vector<_float4x4>& WorldMatrices)
{
	if (m_bDead && GI->Get_CurrentLevel() != LEVEL_TOOL)
		return S_OK;

	if (FAILED(Bind_ShaderResource_Instance(pInstancingShader)))
		return E_FAIL;

	if (m_eType == EFFECT_TYPE::EFFECT_MESH)
	{
		_uint iNumMesh = m_pModelCom->Get_NumMeshes();
		for (_uint i = 0; i < iNumMesh; ++i)
		{
			if (FAILED(m_pModelCom->Render_Instancing(pInstancingShader, i, pInstancingBuffer, WorldMatrices, m_iPassIndex)))
				return E_FAIL;
		}
	}
	else
	{
		pInstancingShader->Begin(m_iPassIndex);
		pInstancingBuffer->Render(WorldMatrices, m_pVIBufferCom);
	}
	return S_OK;
}

HRESULT CEffect::Save_EffectInfo()
{
	return S_OK;
}

void CEffect::Set_EffectDesc(const EFFECT_DESC& tDesc)
{
	m_tEffectDesc = tDesc;
	m_iDiffuseTextureIdx = m_pDiffuseTextureCom->Find_Index(m_tEffectDesc.strDiffuseTetextureName);
	m_iAlphaTextureIdx = m_pAlphaTextureCom->Find_Index(m_tEffectDesc.strAlphaTexturName);
}

void CEffect::Set_Gravity(_bool bGravity)
{
	m_pRigidBodyCom->Set_Gravity(bGravity);
}

_bool CEffect::Is_Gravity()
{
	return m_pRigidBodyCom->Is_Gravity();
}

void CEffect::Set_MoveDir(_vector vDir)
{
	XMStoreFloat3(&m_tEffectDesc.vMoveDir, XMVector3Normalize(vDir));
}

void CEffect::Reset_Effect()
{
	m_tEffectDesc = m_tOriginEffectDesc;
	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	m_pRigidBodyCom->Set_Velocity({ 0.f, 0.f, 0.f });

	m_iDiffuseTextureIdx = m_pDiffuseTextureCom->Find_Index(m_tEffectDesc.strDiffuseTetextureName);
	m_iAlphaTextureIdx = m_pAlphaTextureCom->Find_Index(m_tEffectDesc.strAlphaTexturName);
	XMStoreFloat4x4(&m_ParentMatrix, XMMatrixIdentity());

	m_tEffectDesc.vUVFlow = { 0.f, 0.f };
	m_fAccUVFlow = { 0.f, 0.f };
	m_vUVIndex = { 0.f, 0.f };

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

HRESULT CEffect::Bind_ShaderResource_Instance(CShader* pShader)
{
	/*if (FAILED(pShader->Bind_RawValue("g_fMaxCountX", &m_tEffectDesc.fMaxCountX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue("g_fMaxCountY", &m_tEffectDesc.fMaxCountY, sizeof(_float))))
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue("g_fAlpha", &m_tEffectDesc.fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue("g_fUVIndex", &m_vUVIndex, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue("g_fAdditiveDiffuseColor", &m_tEffectDesc.vAdditiveDiffuseColor, sizeof(_float3))))
		return E_FAIL;


	if (FAILED(pShader->Bind_RawValue("g_fUVFlow", &m_fAccUVFlow, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue("g_iCutUV", &m_tEffectDesc.bCutUV, sizeof(_int))))
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue("g_vBloomPower", &m_tEffectDesc.vBloomPower, sizeof(_float3))))
		return E_FAIL;*/


	if (FAILED(pShader->Bind_RawValue("g_fBlurPower", &m_tEffectDesc.fBlurPower, sizeof(_float))))
		return E_FAIL;
	{

		if (-1 < m_iDiffuseTextureIdx)
		{
			if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource(pShader, "g_DiffuseTexture", m_iDiffuseTextureIdx)))
				return E_FAIL;
		}


		if (-1 < m_iAlphaTextureIdx)
		{
			if (FAILED(m_pAlphaTextureCom->Bind_ShaderResource(pShader, "g_AlphaTexture", m_iAlphaTextureIdx)))
				return E_FAIL;
		}


		// 둘다 없다면.
		if (-1 == m_iDiffuseTextureIdx && -1 == m_iAlphaTextureIdx)
			m_iPassIndex = 0;

		// 디퓨즈 텍스쳐만 있다면.
		if (-1 != m_iDiffuseTextureIdx && -1 == m_iAlphaTextureIdx)
			m_iPassIndex = 1;

		// 알파 텍스쳐만 있다면.
		if (-1 == m_iDiffuseTextureIdx && -1 != m_iAlphaTextureIdx)
			m_iPassIndex = 2;

		if (-1 != m_iDiffuseTextureIdx && -1 != m_iAlphaTextureIdx)
			m_iPassIndex = 3;
	}


	if (FAILED(pShader->Bind_RawValue("g_ViewMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue("g_ProjMatrix", &GAME_INSTANCE->Get_TransformFloat4x4_TransPose(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
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


	m_pRigidBodyCom->Set_RefHeight(-999.f);
	m_pRigidBodyCom->Set_Gravity(m_bGravity);



	if(m_eType == EFFECT_TYPE::EFFECT_MESH)
	{

		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strModelPrototype, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	else
	{

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
		return nullptr;
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
		return nullptr;
	}

	return pInstance;
}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pAlphaTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pVIBufferCom);
}






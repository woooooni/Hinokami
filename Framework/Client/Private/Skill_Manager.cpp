#include "stdafx.h"
#include "Skill_Manager.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Transform.h"
#include "GameObject.h"
#include "Effect.h"
#include <filesystem>
#include "FileUtils.h"
#include "Utils.h"

IMPLEMENT_SINGLETON(CSkill_Manager)

CSkill_Manager::CSkill_Manager()
{

}

HRESULT CSkill_Manager::Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strEffectPath)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	if (FAILED(Ready_Proto_Effects(strEffectPath)))
		return E_FAIL;
	
	return S_OK;
}

void CSkill_Manager::Tick(_float fTimeDelta)
{

}

HRESULT CSkill_Manager::Generate_Effect(const wstring& strEffectName, _matrix OffsetMatrix, _matrix WorldMatrix, _float fEffectDeletionTime, CGameObject* pOwner)
{

	CGameObject* pGameObject = GI->Clone_GameObject(L"Prototype_Effect_" + strEffectName, LAYER_TYPE::LAYER_EFFECT);
	if (nullptr == pGameObject)
		return E_FAIL;
	
	CEffect* pEffect = dynamic_cast<CEffect*>(pGameObject);
	if (nullptr == pEffect)
		return E_FAIL;



	CEffect::EFFECT_DESC EffectDesc = pEffect->Get_EffectDesc();
	XMStoreFloat4x4(&EffectDesc.OffsetMatrix, OffsetMatrix);
	pEffect->Set_EffectDesc(EffectDesc);


	pEffect->Set_Owner(pGameObject);
	pEffect->Set_DeletionTime(fEffectDeletionTime);


	CTransform* pTransform = pEffect->Get_Component<CTransform>(L"Com_Transform");
	if (pTransform == nullptr)
		return E_FAIL;

	pTransform->Set_WorldMatrix(WorldMatrix);
	
	if (FAILED(GI->Add_GameObject(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_EFFECT, pEffect)))
		return E_FAIL;

	return S_OK;
}




HRESULT CSkill_Manager::Ready_Proto_Effects(const wstring& strEffectPath)
{

	for (auto& p : std::filesystem::directory_iterator(strEffectPath))
	{
		if (p.is_directory())
			Ready_Proto_Effects(p.path());


		wstring strFullPath = CUtils::PathToWString(p.path().wstring());
		_tchar strFileName[MAX_PATH];
		_tchar strFolderName[MAX_PATH];
		_tchar strExt[MAX_PATH];

		_wsplitpath_s(strFullPath.c_str(), nullptr, 0, strFolderName, MAX_PATH, strFileName, MAX_PATH, strExt, MAX_PATH);

		if (0 == lstrcmp(TEXT(".effect"), strExt))
		{
			shared_ptr<CFileUtils> File = make_shared<CFileUtils>();
			File->Open(strFullPath, FileMode::Read);

			string strEffectModelName;
			strEffectModelName = File->Read<string>();

			CEffect::EFFECT_TYPE eType;
			eType = File->Read<CEffect::EFFECT_TYPE>();


			CEffect::EFFECT_DESC EffectDesc = {};
			EffectDesc.strDiffuseTetextureName = CUtils::ToWString(File->Read<string>());
			EffectDesc.strAlphaTexturName = CUtils::ToWString(File->Read<string>());

			EffectDesc.bBillboard = File->Read<_bool>();
			EffectDesc.bCutUV = File->Read<_bool>();

			EffectDesc.fAlpha = File->Read<_float>();
			EffectDesc.fDestAlphaSpeed = File->Read<_float>();
			EffectDesc.fIndexSpeed = File->Read<_float>();
			EffectDesc.fMaxCountX = File->Read<_float>();
			EffectDesc.fMaxCountY = File->Read<_float>();
			EffectDesc.fMoveSpeed = File->Read<_float>();
			EffectDesc.fTurnSpeed = File->Read<_float>();

			EffectDesc.vBlurPower = File->Read<_float2>();
			EffectDesc.vUVFlow = File->Read<_float2>();

			EffectDesc.vMoveDir = File->Read<_float3>();
			EffectDesc.vTurnDir = File->Read<_float3>();


			EffectDesc.vAdditiveDiffuseColor = File->Read<_float3>();
			EffectDesc.OffsetMatrix = File->Read<_float4x4>();


			_bool bLoop, bIncrement, bGravity;
			bLoop = File->Read<_bool>();
			bIncrement = File->Read<_bool>();
			bGravity = File->Read<_bool>();

			if (FAILED(GI->Add_Prototype(wstring(L"Prototype_Effect_") + strFileName, 
				CEffect::Create(m_pDevice, m_pContext, 
					strFileName, 
					eType, 
					CUtils::ToWString(strEffectModelName), 
					EffectDesc, bIncrement, bLoop, bGravity), 
					LAYER_TYPE::LAYER_EFFECT)))
				return E_FAIL;
			
			CGameObject* pObject = GI->Find_Prototype_GameObject(LAYER_TYPE::LAYER_EFFECT, wstring(L"Prototype_Effect_") + strFileName);
			if (pObject == nullptr)
				return E_FAIL;

			CEffect* pEffect = dynamic_cast<CEffect*>(pObject);
			if (pEffect == nullptr)
				return E_FAIL;


		}
	}
	return S_OK;
}

void CSkill_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

#include "stdafx.h"
#include "Effect_Manager.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Transform.h"
#include "GameObject.h"
#include "Effect.h"
#include <filesystem>
#include "FileUtils.h"
#include "Utils.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
{

}

HRESULT CEffect_Manager::Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strEffectPath)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	if (FAILED(Ready_Proto_Effects(strEffectPath)))
		return E_FAIL;
	
	return S_OK;
}

void CEffect_Manager::Tick(_float fTimeDelta)
{

}

HRESULT CEffect_Manager::Generate_Effect(const wstring& strPrototypeEffectName, _matrix TransformMatrix, _float fEffectDeletionTime)
{

	CGameObject* pGameObject = GI->Clone_GameObject(strPrototypeEffectName, LAYER_TYPE::LAYER_EFFECT);
	if (nullptr == pGameObject)
		return E_FAIL;
	
	CEffect* pEffect = dynamic_cast<CEffect*>(pGameObject);
	if (nullptr == pEffect)
		return E_FAIL;

	pEffect->Set_DeletionTime(fEffectDeletionTime);


	CTransform* pTransform = pEffect->Get_Component<CTransform>(L"Com_Transform");
	if (pTransform == nullptr)
		return E_FAIL;

	pTransform->Set_WorldMatrix(TransformMatrix);
	
	if (FAILED(GI->Add_GameObject(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_EFFECT, pEffect)))
		return E_FAIL;

	return S_OK;
}




HRESULT CEffect_Manager::Ready_Proto_Effects(const wstring& strEffectPath)
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
			EffectDesc = File->Read<CEffect::EFFECT_DESC>();

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

void CEffect_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

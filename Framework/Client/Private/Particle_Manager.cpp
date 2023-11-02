#include "stdafx.h"
#include "Particle_Manager.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Transform.h"
#include "GameObject.h"
#include <filesystem>
#include "FileUtils.h"
#include "Utils.h"

IMPLEMENT_SINGLETON(CParticle_Manager)

CParticle_Manager::CParticle_Manager()
{

}

HRESULT CParticle_Manager::Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strParticlePath)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	


	
	return S_OK;
}

void CParticle_Manager::Tick(_float fTimeDelta)
{

}

HRESULT CParticle_Manager::Generate_Effect(const wstring& strPrototypeEffectName, _matrix TransformMatrix)
{
	CGameObject* pEffect = GI->Clone_GameObject(strPrototypeEffectName, LAYER_TYPE::LAYER_EFFECT);

	if (nullptr == pEffect)
		return E_FAIL;

	CTransform* pTransform = pEffect->Get_Component<CTransform>(L"Com_Transform");
	if (pTransform == nullptr)
		return E_FAIL;

	pTransform->Set_WorldMatrix(TransformMatrix);
	
	if (FAILED(GI->Add_GameObject(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_EFFECT, pEffect)))
		return E_FAIL;

	return S_OK;
}




HRESULT CParticle_Manager::Ready_Proto_Effects(const wstring& strEffectPath)
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

		if (0 == lstrcmp(TEXT(".particle"), strExt))
		{
			
			
		}
	}



	

	return S_OK;
}

void CParticle_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

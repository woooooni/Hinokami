#include "stdafx.h"
#include "Effect_Manager.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Transform.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
{

}

HRESULT CEffect_Manager::Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	

	return S_OK;
}

void CEffect_Manager::Tick(_float fTimeDelta)
{

}

HRESULT CEffect_Manager::Generate_Effect(const wstring& strPrototypeEffectName, _matrix TransformMatrix)
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




void CEffect_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

#include "..\Public\GameInstance.h"
#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Model_Manager.h"
#include "Key_Manager.h"
#include "Collision_Manager.h"
#include "Target_Manager.h"
#include "Frustum.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pTimer_Manager(CTimer_Manager::GetInstance())
	, m_pGraphic_Device(CGraphic_Device::GetInstance())
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_pLevel_Manager(CLevel_Manager::GetInstance())
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
	, m_pKey_Manager(CKey_Manager::GetInstance())
	, m_pFont_Manager(CFont_Manager::GetInstance())
	, m_pModel_Manager(CModel_Manager::GetInstance())
	, m_pCollision_Manager(CCollision_Manager::GetInstance())
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pFrustum(CFrustum::GetInstance())
	, m_pSound_Manager(CSound_Manager::GetInstance())
	// , m_pNetwork_Manager(CNetwork_Manager::GetInstance())
	
{
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pKey_Manager);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pModel_Manager);
	Safe_AddRef(m_pCollision_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pSound_Manager);
	// Safe_AddRef(m_pNetwork_Manager);
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, _uint iNumLayerType, 
	const GRAPHIC_DESC& GraphicDesc, 
	_Inout_ ID3D11Device** ppDevice, 
	_Inout_ ID3D11DeviceContext** ppContext, 
	_In_ HWND hWnd,
	_In_ HINSTANCE hInst)
{
	/* 그래픽디바이스 초기화 처리. */
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, ppDevice, ppContext)))
		return E_FAIL;

	/* 사운드디바이스 초기화 처리. */
	/* 입력디바이스 초기화 처리. */

	/* 오브젝트 매니져의 예약 처리. */
	if (FAILED(m_pObject_Manager->Reserve_Manager(iNumLevels, iNumLayerType)))
		return E_FAIL;

	/* 컴포넌트 매니져의 예약 처리. */
	if (FAILED(m_pComponent_Manager->Reserve_Manager(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pKey_Manager->Reserve_Manager(*ppDevice, *ppContext, hWnd)))
		return E_FAIL;

	if (FAILED(m_pInput_Device->Initialize(hInst, hWnd)))
		return E_FAIL;

	if (FAILED(m_pModel_Manager->Reserve_Manager(*ppDevice, *ppContext)))
		return E_FAIL;

	if (FAILED(m_pCollision_Manager->Reserve_Manager()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_Shadow_DSV(*ppDevice, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY)))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

	if (FAILED(m_pSound_Manager->Reserve_Manager()))
		return E_FAIL;
	

	return S_OK;
}

void CGameInstance::Tick(_float fTimeDelta)
{
	m_pInput_Device->Update();
	m_pKey_Manager->Tick(fTimeDelta);
	m_pLevel_Manager->Tick(fTimeDelta);
	m_pObject_Manager->Priority_Tick(fTimeDelta);
	m_pObject_Manager->Tick(fTimeDelta);
  	m_pPipeLine->Tick();
	m_pFrustum->Tick();

}

void CGameInstance::LateTick(_float fTimeDelta)
{
	m_pCollision_Manager->LateTick(fTimeDelta);
	m_pObject_Manager->LateTick(fTimeDelta);
	m_pLevel_Manager->LateTick(fTimeDelta);
}



void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Clear(iLevelIndex);
}

CTimer* CGameInstance::Find_Timer(const wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return nullptr;

	return m_pTimer_Manager->Find_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);	
}

HRESULT CGameInstance::Add_Timer(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Get_TimeDelta(const wstring& strTimerTag)
{

	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Set_TimeScale(const wstring& strTimerTag, _float fTimeScale)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Set_TimeScale(strTimerTag, fTimeScale);
}

_float CGameInstance::Get_TimeScale(const wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Get_TimeScale(strTimerTag);
}

HRESULT CGameInstance::Set_Slow(const wstring& strTimerTag, _float fSlowTime, _float fTimeScale, _bool bForce)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Set_Slow(strTimerTag, fSlowTime, fTimeScale, bForce);
}

ID3D11Device* CGameInstance::Get_Device()
{
	return m_pGraphic_Device->Get_Device();
}

ID3D11DeviceContext* CGameInstance::Get_Context()
{
	return m_pGraphic_Device->Get_Context();
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{

	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	return m_pGraphic_Device->Present();
}

_char CGameInstance::Get_DIKState(_uchar eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKState(eKeyID);
}

_char CGameInstance::Get_DIMKeyState(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMKeyState(eMouseKeyID);
}

_long CGameInstance::Get_DIMMoveState(DIMM eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMMoveState(eMouseMoveID);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;
	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Render_Debug()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Render_Debug();
}

_uint CGameInstance::Get_CurrentLevel()
{
	return m_pLevel_Manager->Get_CurrentLevel();
}

HRESULT CGameInstance::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype, _uint iLayerType)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype, iLayerType);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, _uint iLayerType, const wstring & strPrototypeTag, void* pArg, __out class CGameObject** ppOut)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(iLevelIndex, iLayerType, strPrototypeTag, pArg, ppOut);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const _uint iLayerType, CGameObject* pGameObject)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(iLevelIndex, iLayerType, pGameObject);
}

CGameObject* CGameInstance::Clone_GameObject(const wstring& strPrototypeTag, _uint iLayerType, void* pArg)
{
	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, iLayerType, pArg);
}

CGameObject* CGameInstance::Find_Prototype_GameObject(_uint iLayerType, const wstring& strPrototypeTag)
{
	return m_pObject_Manager->Find_Prototype_GameObject(iLayerType, strPrototypeTag);
}

const map<const wstring, class CGameObject*>& CGameInstance::Find_Prototype_GameObjects(_uint iLayerType)
{
	return m_pObject_Manager->Find_Prototype_GameObjects(iLayerType);
}

CGameObject* CGameInstance::Find_GameObject(_uint iLevelIndex, const _uint iLayerType, const wstring& strObjectTag)
{
	return m_pObject_Manager->Find_GameObject(iLevelIndex, iLayerType, strObjectTag);
}

CGameObject* CGameInstance::Find_GameObject(_uint iLevelIndex, const _uint iLayerType, _int iObjectID)
{
	return m_pObject_Manager->Find_GameObject(iLevelIndex, iLayerType, iObjectID);
}

list<CGameObject*>& CGameInstance::Find_GameObjects(_uint iLevelIndex, const _uint iLayerType)
{
	return m_pObject_Manager->Find_GameObjects(iLevelIndex, iLayerType);
}

HRESULT CGameInstance::Clear_Layer(_uint iLevelIndex, const _uint iLayerType)
{
	return m_pObject_Manager->Clear_Layer(iLevelIndex, iLayerType);
}



HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring& strProtoTypeTag, CComponent* pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strProtoTypeTag, pPrototype);
}

CComponent* CGameInstance::Find_Prototype_Component(_uint iLevelIndex, const wstring& strProtoTypeTag)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;
	return m_pComponent_Manager->Find_Prototype_Component(iLevelIndex, strProtoTypeTag);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& strProtoTypeTag, CGameObject* pOwner, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strProtoTypeTag, pOwner, pArg);
}

HRESULT CGameInstance::Check_Prototype(_uint iLevelIndex, const wstring& strProtoTypeTag)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;
	return m_pComponent_Manager->Check_Prototype(iLevelIndex, strProtoTypeTag);
}

const LIGHTDESC* CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(pDevice, pContext, LightDesc);
}

HRESULT CGameInstance::Add_ShadowLight(_uint iLevelIndex, _vector vEye, _vector vAt, _vector vUp)
{
	return m_pLight_Manager->Add_ShadowLight(iLevelIndex, vEye, vAt, vUp);
}



_float4x4 CGameInstance::Get_ShadowViewMatrix(_uint iLevelIndex)
{
	return m_pLight_Manager->Get_ShadowLightViewMatrix(iLevelIndex);
}

_float4x4 CGameInstance::Get_ShadowLightViewMatrix_Inverse(_uint iLevelIndex)
{
	return m_pLight_Manager->Get_ShadowLightViewMatrix_Inverse(iLevelIndex);
}

HRESULT CGameInstance::Reset_Lights()
{
	return m_pLight_Manager->Reset_Lights();
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eTransformState, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eTransformState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eTransformState);

}

_float4x4 CGameInstance::Get_TransformFloat4x4_TransPose(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4_TransPose(eTransformState);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition();
}

_float4x4 CGameInstance::Get_TransformMatrixInverse_Float4x4(CPipeLine::TRANSFORMSTATE eTransformState)
{
	return m_pPipeLine->Get_TransformMatrixInverse_Float4x4(eTransformState);
}

_matrix CGameInstance::Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrixInverse(eTransformState);
}

HRESULT CGameInstance::Bind_TransformToShader(CShader* pShader, const char* pConstantName, CPipeLine::TRANSFORMSTATE eState)
{
	return m_pPipeLine->Bind_TransformToShader(pShader, pConstantName, eState);
}

KEY_STATE CGameInstance::GetKeyState(KEY _eKey)
{
	if (nullptr == m_pKey_Manager)
		return KEY_STATE::NONE;

	return m_pKey_Manager->GetKeyState(_eKey);
}

const POINT& CGameInstance::GetMousePos()
{
	return m_pKey_Manager->GetMousePos();
}

void CGameInstance::Lock_Mouse()
{
	ShowCursor(false);
	m_pKey_Manager->Lock_Mouse();
}

void CGameInstance::UnLock_Mouse()
{
	ShowCursor(true);
	m_pKey_Manager->UnLock_Mouse();
}


HRESULT CGameInstance::Ready_Model_Data_FromPath(_uint iLevelIndex, _uint eType, const wstring& strFolderPath)
{
	return m_pModel_Manager->Ready_Model_Data_FromPath(iLevelIndex, eType, strFolderPath);
}

HRESULT CGameInstance::Export_Model_Data(CModel* pModel, const wstring& strSubFolderName, wstring strFileName)
{
	return m_pModel_Manager->Export_Model_Data(pModel, strSubFolderName, strFileName);
}

HRESULT CGameInstance::Import_Model_Data(_uint iLevelIndex, const wstring& strProtoTypeTag, _uint eType, wstring strFolderPath, wstring strFileName, __out class CModel** ppOut)
{
	return m_pModel_Manager->Import_Model_Data(iLevelIndex, strProtoTypeTag, eType, strFolderPath, strFileName, ppOut);
}

HRESULT CGameInstance::Export_Model_Data_FromPath(_uint eType, wstring strFolderPath)
{
	return m_pModel_Manager->Export_Model_Data_FromPath(eType, strFolderPath);
}

HRESULT CGameInstance::Add_Fonts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontTag, const wstring& strFontFilePath)
{
	return m_pFont_Manager->Add_Fonts(pDevice, pContext, strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Fonts(const wstring& strFontTag, const wstring& strText, _float2 vPosition, _fvector vColor, _float fAngle, _float2 vOrigin, _float2 vScale)
{
	return m_pFont_Manager->Render_Fonts(strFontTag, strText, vPosition, vColor, fAngle, vOrigin, vScale);
}

HRESULT CGameInstance::Add_CollisionGroup(COLLISION_GROUP eCollisionGroup, CGameObject* pGameObject)
{
	return m_pCollision_Manager->Add_CollisionGroup(eCollisionGroup, pGameObject);
}

void CGameInstance::Reset_CollisionGroup()
{
	m_pCollision_Manager->Reset();
}

_bool CGameInstance::Intersect_Frustum_World(_fvector vWorldPos, _float fRadius)
{
	return m_pFrustum->Intersect_Frustum_World(vWorldPos, fRadius);
}

void CGameInstance::Play_Sound(TCHAR* pSoundKey, CHANNELID eID, _float fVolume, _bool bStop)
{
	m_pSound_Manager->Play_Sound(pSoundKey, eID, fVolume, bStop);
}

void CGameInstance::Play_BGM(TCHAR* pSoundKey, _float fVolume, _bool bStop)
{
	m_pSound_Manager->Play_BGM(pSoundKey, fVolume, bStop);
}

void CGameInstance::Stop_Sound(CHANNELID eID)
{
	m_pSound_Manager->Stop_Sound(eID);
}

void CGameInstance::Stop_All()
{
	m_pSound_Manager->Stop_All();
}

void CGameInstance::Set_ChannelVolume(CHANNELID eID, float fVolume)
{
	m_pSound_Manager->Set_ChannelVolume(eID, fVolume);
}

//void CGameInstance::Set_ServerSession(ServerSessionRef session)
//{
//	m_pNetwork_Manager->Set_ServerSession(session);
//}
//
//void CGameInstance::Send(SendBufferRef sendBuffer)
//{
//	m_pNetwork_Manager->Send(sendBuffer);
//}
//
//bool CGameInstance::Is_Connected()
//{
//	return m_pNetwork_Manager->Is_Connected();
//}
//
//ServerSessionRef& CGameInstance::Get_ServerSession()
//{
//	// TODO: 여기에 return 문을 삽입합니다.
//	return m_pNetwork_Manager->Get_ServerSession();
//}


#pragma region No Use(Camera_Manager)
//HRESULT CGameInstance::Add_Camera(const wstring& strCameraName, CCamera* pCamera)
//{
//	if (nullptr == m_pCamera_Manager)
//		return E_FAIL;
//
//	return m_pCamera_Manager->Add_Camera(strCameraName, pCamera);
//}
//
//HRESULT CGameInstance::Set_MainCamera(const wstring& strCameraName)
//{
//	if (nullptr == m_pCamera_Manager)
//		return E_FAIL;
//
//	
//	return m_pCamera_Manager->Set_MainCamera(strCameraName);
//}
//
//HRESULT CGameInstance::Camera_Clear()
//{
//	if (nullptr == m_pCamera_Manager)
//		return E_FAIL;
//
//	return m_pCamera_Manager->Clear();
//}
#pragma endregion







void CGameInstance::Release_Engine()
{
	CSound_Manager::GetInstance()->DestroyInstance();
	CLevel_Manager::GetInstance()->DestroyInstance();
	CTarget_Manager::GetInstance()->DestroyInstance();
	CObject_Manager::GetInstance()->DestroyInstance();
	CComponent_Manager::GetInstance()->DestroyInstance();
	CTimer_Manager::GetInstance()->DestroyInstance();
	CFrustum::GetInstance()->DestroyInstance();
	CPipeLine::GetInstance()->DestroyInstance();
	CInput_Device::GetInstance()->DestroyInstance();
	CLight_Manager::GetInstance()->DestroyInstance();
	CKey_Manager::GetInstance()->DestroyInstance();
	CFont_Manager::GetInstance()->DestroyInstance();
	CModel_Manager::GetInstance()->DestroyInstance();
	CCollision_Manager::GetInstance()->DestroyInstance();
	CGraphic_Device::GetInstance()->DestroyInstance();
	CGameInstance::GetInstance()->DestroyInstance();
	
}

void CGameInstance::Free()
{
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pModel_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pSound_Manager);
	// Safe_Release(m_pNetwork_Manager);
}

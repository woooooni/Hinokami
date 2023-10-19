#pragma once

#include "Component_Manager.h"

/* Ŭ���̾�Ʈ�����ڰ� ������ ����� �̿��ϰ����� �� �����ϴ� ��ü.  */
/* Ŭ���̾�Ʈ�� ��������� �Լ����� ��� �����ϰ� �ִ´�. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.GameInstance */
	HRESULT Initialize_Engine(_uint iNumLevels, _uint iNumLayerType, 
		const GRAPHIC_DESC& GraphicDesc, 
		_Inout_ ID3D11Device** ppDevice, 
		_Inout_ ID3D11DeviceContext** ppContext, 
		_In_ HWND hWnd,
		_In_ HINSTANCE hInst);

	void Tick(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

public: /* For.Timer_Manager */
	_float Compute_TimeDelta(const wstring& strTimerTag);
	HRESULT	Add_Timer(const wstring& strTimerTag);

public: /* For.Graphic_Device */
	ID3D11Device* Get_Device();
	ID3D11DeviceContext* Get_Context();
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);	
	HRESULT Clear_DepthStencil_View();	
	HRESULT Present();
	
public: /* For.Input_Device */
	_char Get_DIKState(_uchar eKeyID);
	_char Get_DIMKeyState(DIMK eMouseKeyID);
	_long Get_DIMMoveState(DIMM eMouseMoveID);


public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	HRESULT Render_Debug();

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const wstring & strPrototypeTag, class CGameObject* pPrototype, _uint iLayerType);
	HRESULT Add_GameObject(_uint iLevelIndex, const _uint iLayerType, const wstring & strPrototypeTag, void* pArg = nullptr, __out class CGameObject** ppOut = nullptr);
	HRESULT Add_GameObject(_uint iLevelIndex, const _uint iLayerType, class CGameObject* pGameObject);
	class CGameObject* Clone_GameObject(const wstring & strPrototypeTag, _uint iLayerType, void* pArg = nullptr);
	class CGameObject* Find_Prototype_GameObject(_uint iLayerType, const wstring & strPrototypeTag);
	const map<const wstring, class CGameObject*>& Find_Prototype_GameObjects(_uint iLayerType);
	class CGameObject* Find_GameObejct(_uint iLevelIndex, const _uint iLayerType, const wstring & strObjectTag);
	class CGameObject* Find_GameObejct(_uint iLevelIndex, const _uint iLayerType, _int iObjectID);
	list<CGameObject*>& Find_GameObjects(_uint iLevelIndex, const _uint iLayerType);
	HRESULT Clear_Layer(_uint iLevelIndex, const _uint iLayerType);

public: /* For. Componenet_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring & strProtoTypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring & strProtoTypeTag, class CGameObject* pOwner, void* pArg = nullptr);
	HRESULT Check_Prototype(_uint iLevelIndex, const wstring & strProtoTypeTag);

public: /* For.Light_Manager */
	const LIGHTDESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc);
	HRESULT Reset_Lights();


public: /* For.PipeLine */
	void Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix);
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4 Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4 Get_TransformFloat4x4_TransPose(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4 Get_CamPosition();
	_matrix Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE eTransformState) const;
	HRESULT Bind_TransformToShader(CShader* pShader, const char* pConstantName, CPipeLine::TRANSFORMSTATE eState);

/* For. KeyManager */
public:
	KEY_STATE GetKeyState(KEY _eKey);
	const POINT& GetMousePos();

public:
	/* For. Model_Manager */
	HRESULT Ready_Model_Data_FromPath(_uint iLevelIndex, _uint eType, const wstring & strFolderPath);
	HRESULT Export_Model_Data(class CModel* pModel, const wstring & strSubFolderName, wstring strFileName);
	HRESULT Import_Model_Data(_uint iLevelIndex, const wstring & strProtoTypeTag, _uint eType, wstring strFolderPath, wstring strFileName, __out class CModel** ppOut = nullptr);
	HRESULT Export_Model_Data_FromPath(_uint eType, wstring strFolderPath);

/* For. CameraManager*/
//public:
//	HRESULT Add_Camera(const wstring & strCameraName, class CCamera * pCamera);
//	HRESULT Set_MainCamera(const wstring & strCameraName);
//	HRESULT Camera_Clear();

/* For. Font_Manager */
public:
	HRESULT Add_Fonts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strFontTag, const wstring & strFontFilePath);
	HRESULT Render_Fonts(const wstring & strFontTag, const _tchar * strText, _float2 vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f),
		_float fAngle = 0.f, _float2 vOrigin = _float2(0.f, 0.f), _float2 vScale = _float2(1.f, 1.f));


/* For. Network_Manager */
//public:
//	void Set_ServerSession(ServerSessionRef session);
//	void Send(SendBufferRef sendBuffer);
//	bool Is_Connected();
//	ServerSessionRef& Get_ServerSession();


private:
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	// class CCamera_Manager*			m_pCamera_Manager = { nullptr };
	class CKey_Manager*				m_pKey_Manager = { nullptr };
	class CModel_Manager*			m_pModel_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	// class CNetwork_Manager*			m_pNetwork_Manager = { nullptr };
public:
	static void Release_Engine();
	virtual void Free() override;
};

END
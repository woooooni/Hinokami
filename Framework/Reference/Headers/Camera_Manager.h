#pragma once
#include "Base.h"
#include "Camera.h"


BEGIN(Engine)

class CCamera_Manager : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager);
	
private:
	CCamera_Manager();
	virtual ~CCamera_Manager();

public:
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);

public:
	HRESULT Add_Camera(const wstring& strCameraName, CCamera* pCamera);
	HRESULT Set_MainCamera(const wstring& strCameraName);
	HRESULT Clear();

private:
	CCamera* Find_Camera(const wstring& strCameraName);



private:
	CCamera* m_pMainCamera = nullptr;
	map<wstring, CCamera*> m_Cameras;

public:
	void Free() override;
};

END


#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Camera.h"

BEGIN(Client)
class CCamera_Manager : public CBase
{	
	DECLARE_SINGLETON(CCamera_Manager)

public:
	enum CAMERA_TYPE { GAME_PLAY, TOOL, TYPE_END};

private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);

public:
	void Set_MainCamera(CAMERA_TYPE eCameraType) {
		if (eCameraType >= CAMERA_TYPE::TYPE_END)
		{
			MSG_BOX("Failed Set MainCamera");
			return;
		}
			
		m_pMainCamera = m_pCameras[eCameraType];
	}

	class CCamera* Get_MainCamera() { return m_pMainCamera; }

private:
	class CCamera* m_pMainCamera = nullptr;
	class CCamera* m_pCameras[CAMERA_TYPE::TYPE_END];

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

private:
	HRESULT Ready_Cameras();

public:
	virtual void Free() override;
};

END

#pragma once

/* 클라에서 정의할 카메라들의 부모. */
/* 모든 카메라가 가져야할 기본적인 기능을 보유한다. */
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	enum CAMERA_STATE { BASIC, CUTSCENE_START_DEFENCE, CUTSCENE_START_TRAIN_BOSS, CUTSCENE_START_FINAL_BOSS, CUTSCENE_KYOJURO_SKILL_0, CUTSCENE_ZENITSU_SKILL_0, STATE_END };

public:
	typedef struct tagCameraDesc
	{
		_float4		vEye, vAt;
		_float		fFovy, fAspect, fNear, fFar;

		CTransform::TRANSFORMDESC		TransformDesc;
	} CAMERADESC;

	typedef struct tagCamShake
	{
		_float fDuration = 0.f;
		_float fForce = 0.f;
		
		_float fAccTime = 0.f;
		_bool bEnd = true;
	} CAM_SHAKE;

	typedef struct tagCamBeat
	{
		_float fDestfFovy;
		_float fAspect;
		_float fNear;
		_float fFar;
	} CAM_BEAT;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, wstring strObjTag, _uint iObjectID);
	CCamera(const CCamera& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Tick_Basic(_float fTimeDelta);
	virtual void Tick_CutScene(_float fTimeDelta);
	virtual void Tick_Skill(_float fTimeDelta);

public:
	void Cam_Shake(_float fDuration, _float fForce);
	void Set_Camera_State(CCamera::CAMERA_STATE eState)
	{
		m_eCurrState = eState;
	}

public:
	HRESULT Set_TargetTransform(class CTransform* pTargetTransform) 
	{ 
		if (nullptr == pTargetTransform)
			return E_FAIL;

		
		m_pTargetTransform = pTargetTransform; 
		Safe_AddRef(m_pTargetTransform);
		return S_OK;
	}



protected:
	class CTransform*			m_pTransformCom = nullptr;
	class CTransform*			m_pTargetTransform = nullptr;


	CAMERADESC					m_CameraDesc;
	CAM_SHAKE					m_tShakeDesc;

	CAMERA_STATE				m_eCurrState = CAMERA_STATE::BASIC;

	
protected:
	virtual HRESULT Ready_Components() override;


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free();
};

END
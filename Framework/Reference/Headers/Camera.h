#pragma once

/* Ŭ�󿡼� ������ ī�޶���� �θ�. */
/* ��� ī�޶� �������� �⺻���� ����� �����Ѵ�. */
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{

public:
	typedef struct tagCameraDesc
	{
		_float4		vEye, vAt;
		_float		fFovy, fAspect, fNear, fFar;

		CTransform::TRANSFORMDESC		TransformDesc;
	}CAMERADESC;

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
	virtual HRESULT Render(class CVIBuffer_Instancing* pInstanceBuffer, const vector<_float4x4>& WorldMatrices) override;

public:
	void Cam_Shake(const CAM_SHAKE& pCameraShakeDesc);

public:
	HRESULT Set_TargetTransform(class CTransform* pTargetTransform) 
	{ 
		if (nullptr == pTargetTransform)
			return E_FAIL;

		m_pTargetTransform = pTargetTransform; 
		return S_OK;
	}



protected:
	class CTransform*			m_pTransformCom = nullptr;
	class CTransform*			m_pTargetTransform = nullptr;


	CAMERADESC					m_CameraDesc;
	CAM_SHAKE					m_tShakeDesc;

	
protected:
	virtual HRESULT Ready_Components() override;


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free();
};

END
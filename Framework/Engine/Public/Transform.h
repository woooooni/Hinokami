#pragma once

#include "Engine_Defines.h"
#include "Component.h"
#include "Lock.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

	typedef struct tagTransformDesc
	{
		_float			fSpeedPerSec;
		_float			fRotationPerSec;
	}TRANSFORMDESC;


private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

	USE_LOCK

public:
	_vector Get_State(STATE eState) {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
	}
	_vector Get_Look() {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_LOOK][0]);
	}
	_vector Get_Right() {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_RIGHT][0]);
	}
	_vector Get_Up() {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_UP][0]);
	}
	_vector Get_Position() {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_POSITION][0]);
	}

	_matrix Get_WorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	void Set_WorldMatrix(_fmatrix WorldMatrix) {
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}

	_float4x4 Get_WorldFloat4x4() const {
		return m_WorldMatrix;
	}

	_float4x4 Get_WorldFloat4x4_TransPose() const {
		_float4x4	WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(Get_WorldMatrix()));
		return WorldMatrix;
	}

	_matrix Get_WorldMatrixInverse() const {
		return XMMatrixInverse(nullptr, Get_WorldMatrix());
	}

public:
	void Set_State(STATE eState, _fvector vState);

	_float Get_TickPerSecond() { return m_TransformDesc.fSpeedPerSec; }
	void Set_TickPerSecond(_float fTickPerSecond) { m_TransformDesc.fSpeedPerSec = fTickPerSecond; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Bind_ShaderResources(class CShader* pShader, const char* pConstantName);

public:
	void Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Dir(_fvector vDir, _float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta, class CNavigation* pNavigation = nullptr);

	void Set_Position(_vector vPosition, _float fTimeDelta, class CNavigation* pNavigation = nullptr, _bool* bMovable = nullptr);
	void Set_Scale(_fvector vScaleInfo);
	void Set_Sclae(_float3 vScale);
	_float3 Get_Scale();

	_float3 Get_Rotaion_Degree();
	_float3 Get_Rotation_Radian();

	void Set_Rotation(_fvector vRadianEulerAngle);

	void Turn(_fvector vAxis, _float fTimeDelta);
	void Turn(_fvector vAxis, _float fSpeed, _float fTimeDelta);
	void Rotation(_fvector vAxis, _float fRadian);
	void Rotation_Acc(_fvector vAxis, _float fRadian);

	void LookAt(_fvector vAt);
	void LookAt_ForLandObject(_fvector vAt);
	void Move(_fvector vTargetPos, _float fTimeDelta, _float fLimitDistance = 0.1f);

	const TRANSFORMDESC& Get_TransformDesc() { return m_TransformDesc; }
	void Set_TransformDesc(const TRANSFORMDESC& TransformDesc) { m_TransformDesc = TransformDesc; }

private:
	_float4x4				m_WorldMatrix;
	_float3					m_vPrevSlidingDir = { 1.f, 0.f, 0.f };
	TRANSFORMDESC			m_TransformDesc;

private:
	_float3 ToEulerAngles(_vector Quaternion);

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
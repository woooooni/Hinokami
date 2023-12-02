#pragma once

#include "Engine_Defines.h"
#include "Base.h"

/* 클라이엉ㄴ트에서 제작할 다양한 게임오브젝트들의 부모가된다. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
	
protected:
	/* 원형을 생성할 때 */
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag, _int iObjectType);

	/* 사본을 생성할 때 */
	CGameObject(const CGameObject& rhs); /* 복사 생성자. */
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Tick(_float fTimeDelta) {};
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();

	virtual HRESULT Render_Instance(class CShader* pInstancingShader, class CVIBuffer_Instancing* pInstancingBuffer, const vector<_float4x4>& WorldMatrices)		{ return S_OK; }
	virtual HRESULT Render_Instance_Shadow(class CShader* pInstancingShader, class CVIBuffer_Instancing* pInstancingBuffer, const vector<_float4x4>& WorldMatrices) { return S_OK; }

public:
	virtual void Enter_Scene() { };
	virtual void Return_Pool() { };

public:
	virtual void Collision_Enter(const COLLISION_INFO& tInfo) {};
	virtual void Collision_Continue(const COLLISION_INFO& tInfo) {};
	virtual void Collision_Exit(const COLLISION_INFO& tInfo) {};

public:
	template<typename T>
	T* Get_Component(const wstring& strComponentTag)
	{
		auto iter = m_Components.find(strComponentTag);

		if (iter == m_Components.end())
			return nullptr;

		return dynamic_cast<T*>(iter->second);
	}

public:
	virtual HRESULT SetUp_State(_fmatrix StateMatrix) { return S_OK; }


public:
	void Set_PrototypeTag(const wstring& strPrototypeTag) { m_strPrototypeTag = strPrototypeTag; }
	const wstring& Get_PrototypeTag() { return m_strPrototypeTag; }

	void Set_ObjectTag(const wstring& strTag) { m_strObjectTag = strTag; }
	const wstring& Get_ObjectTag() { return m_strObjectTag; }
	
	void Set_ObjectID(_uint iID) { m_iObjectID = iID; }
	_uint Get_ObjectID() { return m_iObjectID; }
	_uint Get_ObjectType() { return m_iObjectType; }

	void Set_Dead(_bool bDead) { m_bDead = bDead; }
	_bool Is_Dead() { return m_bDead; }

	void Reserve_Dead(_bool bReserve) { m_bReserveDead = bReserve; }
	_bool Is_ReserveDead() { return m_bReserveDead; }

	void Set_NaviObject(_bool bNaviGation) { m_bNaviObject = bNaviGation; }
	_bool Is_NaviObject() { return m_bNaviObject; }

public:
	vector<class CCollider*>& Get_Collider(_uint eDetectionType) { return m_Colliders[eDetectionType]; }
	HRESULT Add_Collider(_uint iLevelIndex, _uint eColliderType, _uint eDetectionType, void* pArg);
	virtual HRESULT Set_ActiveColliders(_uint eDetectionType, _bool bActive);


	virtual HRESULT Set_Collider_AttackMode(_uint eAttackMode, _float fAirBornPower, _float fPushPower, _float fDamage, _bool bHitLag = true);

protected:
	void LateUpdate_Collider(_float fTimedelta);
	void Render_Collider();
	

protected:
	virtual HRESULT Ready_Components() PURE;
	HRESULT Add_Component(const wstring& pComponentTag, class CComponent* pComponent);
	HRESULT Add_Component(_uint iLevelIndex, const wstring& pPrototypeTag, const wstring& pComponentTag, CComponent** ppOut, void* pArg = nullptr);
	HRESULT Compute_CamZ(_fvector vWorldPos);




protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

protected:
	map<wstring, class CComponent*> m_Components;
	map<_uint, vector<class CCollider*>> m_Colliders;

protected:
	_float				m_fCamDistance = 0.f;
	wstring				m_strPrototypeTag = L"";
	wstring				m_strObjectTag = L"";

	static				_uint g_ObjecId;
	_int				m_iObjectID = -1;
	_int				m_iObjectType = -1;

	_bool				m_bReserveDead = false;
	_bool				m_bDead = false;


protected:
	_bool m_bNaviObject = false;



protected:
	CComponent* Find_Component(const wstring & strComponentTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
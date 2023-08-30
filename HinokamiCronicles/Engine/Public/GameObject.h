#pragma	 once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent* Get_ComponentPtr(const wstring& strComponentTag);
	_float Get_CamDistance() const {
		return m_fCamDistance;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	virtual HRESULT SetUp_State(_fmatrix StateMatrix) { return S_OK; }



protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

protected: /* 객체에게 추가된 컴포넌트들을 키로 분류하여 보관한다. */
	map<const wstring, class CComponent*>			m_Components;
	typedef map<const wstring, class CComponent*>	COMPONENTS;

protected:
	_float				m_fCamDistance = 0.f;

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, class CComponent** ppOut, void* pArg = nullptr);
	HRESULT Compute_CamZ(_fvector vWorldPos);
	


private:
	class CComponent* Find_Component(const wstring& strComponentTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
#pragma once

/* Ư�� ������ ���Ǵ� ��ü������ ��Ƽ� �����صγ�. */
/* �����ϰ� �մ� ��ü���� Tick�� ȣ�����ش�. */

#include "Base.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_ComponentPtr(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iLayerIndex);

public:
	HRESULT Reserve_Container(_uint iNumLevels);
	class CGameObject* Clone_GameObject(const wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObjectToLayer(const wstring& strPrototypeTag, _uint iLevelIndex, const wstring& strLayerTag, void* pArg = nullptr);

public:
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Clear(_uint iLevelIndex);
	

private: /* ������ü���� ������ Ŀ���̳ʤ� */
	map<const wstring, class CGameObject*>			m_Prototypes;
	typedef map<const wstring, class CGameObject*>	PROTOTYPES;


private: /* �纻��ü���� ������ Ŀ���̳ʤ� */
	_uint											m_iNumLevels = 0;
	map<const wstring, class CLayer*>*				m_pLayers = nullptr;
	typedef map<const wstring, class CLayer*>		LAYERS;

private:
	class CGameObject* Find_Prototype(const wstring& strPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);



public:
	virtual void Free() override;
};

END
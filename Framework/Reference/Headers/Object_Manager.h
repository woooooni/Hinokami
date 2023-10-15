#pragma once

#include "Base.h"

/* ���ӳ��� ���� ��ü���� ���������� ������ü���� �����Ѵ�.(����) */
/* ���ӳ��� ���� ��ü���� �����Ѵ�.(�纻) */


BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Reserve_Manager(_uint iNumLevels, _uint iNumLayerTypes);
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype, _uint iLayerType);
	HRESULT Add_GameObject(_uint iLevelIndex, const _uint iLayerType, const wstring& strPrototypeTag,  void* pArg, __out class CGameObject** ppOut = nullptr);
	HRESULT Add_GameObject(_uint iLevelIndex, const _uint iLayerType, class CGameObject* pGameObject);
	class CGameObject* Clone_GameObject(const wstring& strPrototypeTag, _uint iLayerType, void* pArg = nullptr);
	
public:
	const map<const wstring, class CGameObject*>& Find_Prototype_GameObjects(_uint iLayerType);
	class CGameObject* Find_Prototype_GameObject(_uint iLayerType, const wstring& strPrototypeTag);
	class CGameObject* Find_GameObejct(_uint iLevelIndex, const _uint iLayerType, _int iObjectID);
	class CGameObject* Find_GameObejct(_uint iLevelIndex, const _uint iLayerType, const wstring& strObjectTag);
	list<CGameObject*>& Find_GameObjects(_uint iLevelIndex, const _uint iLayerType);

public:
	HRESULT Clear_Layer(_uint iLevelIndex, const _uint iLayerType);

public:
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

private:
	/* ������ü���� �������� �����ұ�?! */
	map<const wstring, class CGameObject*>*				m_pPrototypes;
	_uint	m_iNumLayer = { 0 };

private:
	/* �纻��ü���� �������� �׷�(CLayer)��� �����Ѵ�. */
	_uint								m_iNumLevels = { 0 };
	
	vector<class CLayer*>*				m_pLayers = { nullptr };
	typedef vector<class CLayer*>*		LAYERS;

private:
	class CGameObject* Find_Prototype(const wstring& strPrototypeTag, _uint iLayerType);
	class CLayer* Find_Layer(_uint iLevelIndex, _uint iLayerType);

public:
	virtual void Free() override;
};

END
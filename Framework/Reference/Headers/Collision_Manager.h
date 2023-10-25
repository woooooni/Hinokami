#pragma once

#include "Base.h"

BEGIN(Engine)


union COLLIDER_ID
{
	struct
	{
		UINT iLeft_id;
		UINT iRight_id;
	};
	ULONGLONG ID;
};

class CCollider;
class CCollision_Manager final : public CBase
{
	DECLARE_SINGLETON(CCollision_Manager)

public:
	typedef struct tagCollisionObject
	{
		class CGameObject* pLeft;
		class CGameObject* pRight;
	} COLLISION_OBJECT;


public:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	HRESULT Reserve_Manager();
	void LateTick(_float fTimeDelta);


public:
	HRESULT Add_CollisionGroup(COLLISION_GROUP, class CGameObject* pGameObject);
	void Reset() { memset(m_arrCheck, 0, sizeof(_uint) * (_uint)COLLISION_GROUP::GROUP_END); }

private:
	map<ULONGLONG, bool>		m_mapColInfo;
	_uint						m_arrCheck[(_uint)COLLISION_GROUP::GROUP_END];
	vector<class CGameObject*>	m_CollisionObjects[(_uint)COLLISION_GROUP::GROUP_END];

private:
	HRESULT Reserve_CheckGroup(COLLISION_GROUP eLeft, COLLISION_GROUP eRight);

	void Collision_Update(COLLISION_GROUP eLeft, COLLISION_GROUP eRight);
	_bool Is_Collision(CCollider* pLeft, CCollider* pRight);


public:
	virtual void Free() override;
};

END
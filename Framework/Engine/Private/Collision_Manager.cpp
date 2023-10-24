#include "..\Public\Collision_Manager.h"
#include "GameObject.h"
#include "Collider.h"

IMPLEMENT_SINGLETON(CCollision_Manager)


CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Reserve_Manager()
{

	Reserve_CheckGroup(COLLISION_GROUP::CHATACTER, COLLISION_GROUP::MONSTER);
	Reserve_CheckGroup(COLLISION_GROUP::CHATACTER, COLLISION_GROUP::PROP);
	Reserve_CheckGroup(COLLISION_GROUP::PROP, COLLISION_GROUP::MONSTER);

	for (_uint i = 0; i < COLLISION_GROUP::GROUP_END; ++i)
		m_CollisionObjects[i].reserve(300);



	return S_OK;
}

HRESULT CCollision_Manager::Reserve_CheckGroup(COLLISION_GROUP eLeft, COLLISION_GROUP eRight)
{
	_uint iRow = (_uint)eLeft;
	_uint iCol = (_uint)eRight;

	if (iCol < iRow)
	{
		iRow = (_uint)eRight;
		iCol = (_uint)eLeft;
	}

	if (m_arrCheck[iRow] & (1 << iCol))
	{
		m_arrCheck[iRow] &= ~(1 << iCol);
	}
	else
	{
		m_arrCheck[iRow] |= (1 << iCol);
	}

	return S_OK;
}

void CCollision_Manager::Late_Tick(_float fTimeDelta)
{
	for (_uint iRow = 0; iRow < (_uint)COLLISION_GROUP::GROUP_END; iRow++)
	{
		for (UINT iCol = iRow; iCol < (UINT)COLLISION_GROUP::GROUP_END; iCol++)
		{
			if (m_arrCheck[iRow] & (1 << iCol))
			{
				Collision_Update((COLLISION_GROUP)iRow, (COLLISION_GROUP)iCol);
			}
		}
	}
}

HRESULT CCollision_Manager::Add_CollisionGroup(COLLISION_GROUP eGroup, CGameObject* pGameObject)
{
	
	m_CollisionObjects[eGroup].push_back(pGameObject);

	return S_OK;
}




void CCollision_Manager::Collision_Update(COLLISION_GROUP eLeft, COLLISION_GROUP eRight)
{
	const vector<CGameObject*>& pLeftObject = m_CollisionObjects[eLeft];
	const vector<CGameObject*>& pRightObject = m_CollisionObjects[eRight];


	vector<CGameObject*> IntersectObjects;
	IntersectObjects.reserve(100);

	map<ULONGLONG, bool>::iterator iter;

	for (auto& pLeft : pLeftObject)
	{
		if (pLeft->Is_Dead() || pLeft->Is_ReserveDead() || pLeft->Get_Collider(CCollider::DETECTION_TYPE::BOUNDARY).size() <= 0)
			continue;

		for (auto& pRight : pRightObject)
		{
			if (pRight->Is_ReserveDead() || pRight->Is_Dead() || pRight->Get_Collider(CCollider::DETECTION_TYPE::BOUNDARY).size() <= 0)			
				continue;

			CCollider* pLeftCollider = pLeft->Get_Collider(CCollider::DETECTION_TYPE::BOUNDARY)[0];
			CCollider* pRightCollider = pRight->Get_Collider(CCollider::DETECTION_TYPE::BOUNDARY)[0];

			if (!pLeftCollider->Is_Active() || !pRightCollider->Is_Active())
				continue;

			// gen map key by using union
			COLLIDER_ID ID;
			ID.iLeft_id = min(pLeftCollider->Get_ColliderID(), pRightCollider->Get_ColliderID());
			ID.iRight_id = max(pLeftCollider->Get_ColliderID(), pRightCollider->Get_ColliderID());

			iter = m_mapColInfo.find(ID.ID);
			if (m_mapColInfo.end() == iter)
			{
				m_mapColInfo.insert(make_pair(ID.ID, false));
				iter = m_mapColInfo.find(ID.ID);
			}

			if (Is_Collision(pLeftCollider, pRightCollider))
			{
				if (iter->second)
				{
					if (pLeft->Is_Dead() || pLeft->Is_Dead() || pRight->Is_ReserveDead() || pRight->Is_ReserveDead())
					{
						pLeftCollider->Collision_Exit(pRightCollider);
						pRightCollider->Collision_Exit(pLeftCollider);
						iter->second = false;
					}
					else
					{
						pLeftCollider->Collision_Continue(pRightCollider);
						pRightCollider->Collision_Continue(pLeftCollider);
					}
				}
				else
				{
					if (!pLeft->Is_Dead() && !pRight->Is_Dead() && !pLeft->Is_ReserveDead() && !pRight->Is_ReserveDead())
					{
						pLeftCollider->Collision_Enter(pRightCollider);
						pRightCollider->Collision_Enter(pLeftCollider);
						iter->second = true;
					}
				}
			}
			else
			{
				if (iter->second)
				{
					pLeftCollider->Collision_Enter(pRightCollider);
					pRightCollider->Collision_Enter(pLeftCollider);
					iter->second = false;
				}
			}
		}
	}
}

_bool CCollision_Manager::Is_Collision(CCollider* pLeft, CCollider* pRight)
{
	if (!pLeft->Is_Active() || !pRight->Is_Active())
		return false;

	if (pLeft->Is_Collision(pRight))
		return true;

	return false;
}

void CCollision_Manager::Free()
{
	
}

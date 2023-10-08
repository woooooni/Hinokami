#pragma once

#include "Base.h"

BEGIN(Server)
class CServerObject : public CBase
{
public:
	typedef struct tagObjectInfo
	{
		int32 iObjectID;
		uint32 iObjectType;

		string strName;
		string strPrototypeName;

		uint32 iModelType;
		uint32 iAnimationIndex;
		_float fAnimationPlayTime;

		_float4x4 mWorldMatrix;
	} OBJECT_INFO;



public:
	CServerObject();
	~CServerObject();

public:
	HRESULT Initialize(Protocol::S_CREATE_OBJECT& pkt);

public:
	const OBJECT_INFO& Get_ObjectInfo() { READ_LOCK return m_tObjectInfo; }
	void Set_ObjectInfo(const OBJECT_INFO& tObjectInfo) { WRITE_LOCK  m_tObjectInfo = tObjectInfo; }


private:
	OBJECT_INFO m_tObjectInfo;
	USE_LOCK;
public:
	static CServerObject* Create(Protocol::S_CREATE_OBJECT& pkt);
	virtual void Free() override;
};

END
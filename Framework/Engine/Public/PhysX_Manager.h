#pragma once

#include "Base.h"
#include "Collider.h"


BEGIN(Engine)

// PxUserControllerHitReport
//PxSimulationFilterCallback
class CPhysX_Manager final : public CBase, public PxSimulationEventCallback
{
public:
	typedef struct tagPhysXColDesc
	{
		CGameObject* pObj;
		CCollider::OTHERTOMECOLDESC Desc;
	}PHYSXCOLDESC;



	DECLARE_SINGLETON(CPhysX_Manager)
public:
	CPhysX_Manager();
	virtual ~CPhysX_Manager() = default;

public:
	HRESULT Init();
	void Tick(_float fTimeDelta);


public:
	PxRigidDynamic* Create_Box(_float3 vPos, _float3 vExtent, _uint iFlag = 0);
	PxRigidDynamic*	Create_Sphere(_float3 vPos, _float fRad, _uint iFlag = 0);

	PxRigidDynamic* Create_PxBox(_float3 vExtent, _float fWeight, _float fAngleDump, PxMaterial* pMaterial, _float fMaxVel = 0.f);
	PxRigidDynamic* Create_PxSphere(_float3 vExtent, _float fWeight, _float fAngleDump, PxMaterial* pMaterial, _float fMaxVel = 0.f);

	PxRigidDynamic*	Create_JointBox(PxTransform vPos, _float3 vExtent, PxMaterial* pMaterial, _uint iFlag = 0, _bool bKin = false);
	PxJoint* Create_DampedD6(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1);

	PxRigidStatic* Create_Static(_float3 vPos, _float3 vRot, _float3 vExtent, _uint iFlag = 0);


	void Add_Actor(PxActor* pAxtor);
	void Remove_Actor(PxActor* pAxtor);

	PxMaterial*		Create_Material(_float fA, _float fB, _float fC) {
		return m_Physics->createMaterial(fA, fB, fC);
	}

public:
	virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;

	virtual void onWake(PxActor** actors, PxU32 count) override;

	virtual void onSleep(PxActor** actors, PxU32 count) override;

	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)override;

	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;

	virtual void onAdvance(const PxRigidBody*const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override;


private:
	PxDefaultAllocator			m_Allocator;			// 메모리 관리하는 놈?
	PxDefaultErrorCallback		m_ErrorCallback;
	PxFoundation*				m_Foundation = NULL;	// 기초 설정
	PxPhysics*					m_Physics = NULL;		// 이놈으로 여러가지 함
	PxDefaultCpuDispatcher*		m_Dispatcher = NULL;	// 멀쓰 개수?
	PxScene*					m_Scene = NULL;			// 세계(장면)
	PxMaterial*					m_Material = NULL;		// 객체의 재질
	PxCudaContextManager*       m_pCudaContextManager = NULL;



		
public:
	virtual void Free() override;

};

END
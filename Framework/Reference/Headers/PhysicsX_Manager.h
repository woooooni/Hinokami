#pragma once

#include "Base.h"
#include "Collider.h"


BEGIN(Engine)

class CPhysicsX_Manager final : public CBase, public PxSimulationEventCallback
{
public:
	DECLARE_SINGLETON(CPhysicsX_Manager)
public:
	CPhysicsX_Manager();
	virtual ~CPhysicsX_Manager() = default;

public:
	HRESULT Reserve_Manager();
	void Tick(_float fTimeDelta);


public:
	PxRigidDynamic* Create_Box(_float3 vPos, _float3 vExtent, _uint iFlag = 0);
	PxRigidDynamic*	Create_Sphere(_float3 vPos, _float fRad, _uint iFlag = 0);

	PxRigidDynamic* Create_PxBox(_float3 vExtent, _float fWeight, _float fAngleDump, PxMaterial* pMaterial, _float fMaxVel = 0.f);
	PxRigidDynamic* Create_PxSphere(_float3 vExtent, _float fWeight, _float fAngleDump, PxMaterial* pMaterial, _float fMaxVel = 0.f);


	void Add_Actor(PxActor* pAxtor);
	void Remove_Actor(PxActor* pAxtor);

	PxMaterial*		Create_Material(_float fA, _float fB, _float fC) {
		return m_Physics->createMaterial(fA, fB, fC);
	}

public:
	virtual void onConstraintBreak(PxConstraintInfo* Constrains, PxU32 Count) override;
	virtual void onWake(PxActor** Actors, PxU32 Count) override;
	virtual void onSleep(PxActor** Actors, PxU32 Count) override;
	virtual void onContact(const PxContactPairHeader& PairHeader, const PxContactPair* Pairs, PxU32 bPairs) override;
	virtual void onTrigger(PxTriggerPair* Pairs, PxU32 Count) override;
	virtual void onAdvance(const PxRigidBody*const* BodyBuffer, const PxTransform* TransformBuffer, const PxU32 Count) override;


private:
	PxDefaultAllocator			m_Allocator;			// 메모리 관리용..?
	PxDefaultErrorCallback		m_ErrorCallback;
	PxFoundation*				m_Foundation = NULL;	// 기초 설정
	PxPhysics*					m_Physics = NULL;		// 코어가 되는거 같음.
	PxDefaultCpuDispatcher*		m_Dispatcher = NULL;	// 몰..루?
	PxScene*					m_Scene = NULL;			// 세계(장면)
	PxMaterial*					m_Material = NULL;		// 객체의 재질
	PxCudaContextManager*       m_pCudaContextManager = NULL;



		
public:
	virtual void Free() override;

};

END
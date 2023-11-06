#include "PhysicsX_Manager.h"
#include "GameInstance.h"

#include "GameObject.h"

PxFilterFlags FilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// 1: 피직스
	// 2: 플레이어 OBB
	// 9: 머리카락
	// 10: 머리카락 컷 박스 
	
	if (
		// 플레이어 OBB와 피직스 물체 충돌 X
		(1 == filterData0.word0 && 2 == filterData1.word0)
		|| (2 == filterData0.word0 && 1 == filterData1.word0)
		|| (2 == filterData0.word0 && 10 == filterData1.word0)
		|| (10 == filterData0.word0 && 2 == filterData1.word0))
	{
		// 플레이어 박스 && 상자 박스
		// 피직스 호출 X
		// pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	// 9 머리, 10 머리 컷 박스
	if (9 == filterData0.word0 || 10 == filterData1.word0 || 10 == filterData0.word0 || 9 == filterData1.word0)
	{
		if ((9 == filterData0.word0 && 10 == filterData1.word0) || (10 == filterData0.word0 && 9 == filterData1.word0)
			|| (9 == filterData0.word0 && 9 == filterData1.word0))
			pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
	}
	else
	{
		pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
	}


	return PxFilterFlag::eDEFAULT;
}


IMPLEMENT_SINGLETON(CPhysicsX_Manager)


CPhysicsX_Manager::CPhysicsX_Manager()
{

}

HRESULT CPhysicsX_Manager::Reserve_Manager()
{
	m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale());

	PxSceneDesc SceneDesc(m_Physics->getTolerancesScale());
	SceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	m_Dispatcher = PxDefaultCpuDispatcherCreate(4);
	if (!m_Dispatcher)
		return E_FAIL;
	SceneDesc.cpuDispatcher = m_Dispatcher;
	SceneDesc.filterShader = FilterShader;
	SceneDesc.simulationEventCallback = this;
	SceneDesc.kineKineFilteringMode = PxPairFilteringMode::eKEEP;
	SceneDesc.solverType = PxSolverType::eTGS;


	m_Scene = m_Physics->createScene(SceneDesc);
	m_Material = m_Physics->createMaterial(0.f, 0.f, 0.f);

	return S_OK;
}

void CPhysicsX_Manager::Tick(_float fTimeDelta)
{
	if (1.f / 144.f < fTimeDelta)
		fTimeDelta = 1.f / 144.f;

	m_Scene->simulate(fTimeDelta);
	m_Scene->fetchResults(true);
	
}

PxRigidDynamic* CPhysicsX_Manager::Create_Box(_float3 vPos, _float3 vExtent, _uint iFlag)
{
	// 박스를 만든다.
	PxShape* shape = m_Physics->createShape(PxBoxGeometry(vExtent.x * 0.5f, vExtent.y * 0.5f, vExtent.z * 0.5f), *m_Material);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);

	shape->setSimulationFilterData(PxFilterData(iFlag, 0, 0, 0));

	PxTransform localTm(PxVec3(vPos.x, vPos.y, vPos.z)); // 포지션 설정
	PxRigidDynamic* body = m_Physics->createRigidDynamic(localTm);
	body->attachShape(*shape);
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	PxRigidBodyExt::updateMassAndInertia(*body, 10.f); // 무게, 관성
	m_Scene->addActor(*body);

	shape->release();

	return body;
}

PxRigidDynamic* CPhysicsX_Manager::Create_Sphere(_float3 vPos, _float fRad, _uint iFlag)
{
	// 박스를 만든다.
	PxShape* Shape = m_Physics->createShape(PxSphereGeometry(PxReal(fRad)), *m_Material);
	Shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);

	Shape->setSimulationFilterData(PxFilterData(iFlag, 0, 0, 0));


	PxTransform localTm(PxVec3(vPos.x, vPos.y, vPos.z)); // 포지션 설정
	PxRigidDynamic* body = m_Physics->createRigidDynamic(localTm);
	body->attachShape(*Shape);
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	PxRigidBodyExt::updateMassAndInertia(*body, 0.1f); // 무게, 관성
	m_Scene->addActor(*body);
	Shape->release();

	return body;
}

PxRigidDynamic * CPhysicsX_Manager::Create_PxBox(_float3 vExtent, _float fWeight, _float fAngleDump, PxMaterial* pMaterial, _float fMaxVel)
{

	// 박스를 만든다.
	PxShape* shape = m_Physics->createShape(PxBoxGeometry(vExtent.x * 0.5f, vExtent.y * 0.5f, vExtent.z * 0.5f), *pMaterial);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);

	shape->setSimulationFilterData(PxFilterData(1, 0, 0, 0));

	// 이때의 vPos는 상대적인 포스
	// PxTransform localTm(PxVec3(vPos.x, vPos.y, vPos.z)); // 포지션 설정
	PxRigidDynamic* body = m_Physics->createRigidDynamic(PxTransform(0.f, 0.f, 0.f));
	body->attachShape(*shape);

	body->setAngularDamping(fAngleDump); // 회전을 방해하는 힘

	if(fMaxVel)
		body->setMaxLinearVelocity(fMaxVel); // 최대 속도

	PxRigidBodyExt::updateMassAndInertia(*body, fWeight); // 무게, 관성

	m_Scene->addActor(*body);

	shape->release();

	return body;
}

PxRigidDynamic * CPhysicsX_Manager::Create_PxSphere(_float3 vExtent, _float fWeight, _float fAngleDump, PxMaterial * pMaterial, _float fMaxVel)
{

	PxShape* shape = m_Physics->createShape(PxSphereGeometry(PxReal(vExtent.x)), *m_Material);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	shape->setDensityForFluid(10.f); // 밀도

	shape->setSimulationFilterData(PxFilterData(1, 0, 0, 0));

	PxRigidDynamic* body = m_Physics->createRigidDynamic(PxTransform(0.f, 0.f, 0.f));
	body->attachShape(*shape);


	body->setAngularDamping(fAngleDump);

	if (fMaxVel)
		body->setMaxLinearVelocity(fMaxVel);

	PxRigidBodyExt::updateMassAndInertia(*body, fWeight);

	m_Scene->addActor(*body);

	shape->release();

	return body;
}



void CPhysicsX_Manager::Add_Actor(PxActor * pAxtor)
{
	m_Scene->addActor(*pAxtor);
}

void CPhysicsX_Manager::Remove_Actor(PxActor* pAxtor)
{
	if (pAxtor)
		m_Scene->removeActor(*pAxtor);
}

void CPhysicsX_Manager::onConstraintBreak(PxConstraintInfo* Constrains, PxU32 Count)
{
}

void CPhysicsX_Manager::onWake(PxActor** Actors, PxU32 Count)
{
}

void CPhysicsX_Manager::onSleep(PxActor** Actors, PxU32 Count)
{
}

void CPhysicsX_Manager::onContact(const PxContactPairHeader& PairHeader, const PxContactPair* Pairs, PxU32 bPairs)
{
}

void CPhysicsX_Manager::onTrigger(PxTriggerPair* Pairs, PxU32 Count)
{
}

void CPhysicsX_Manager::onAdvance(const PxRigidBody* const* BodyBuffer, const PxTransform* TransformBuffer, const PxU32 Count)
{
}


void CPhysicsX_Manager::Free()
{

	if (m_Material)
		m_Material->release();
	if (m_Scene)
		m_Scene->release();
	if (m_Dispatcher)
		m_Dispatcher->release();
	if (m_Physics)
		m_Physics->release();
	if (m_Foundation)
		m_Foundation->release();


}





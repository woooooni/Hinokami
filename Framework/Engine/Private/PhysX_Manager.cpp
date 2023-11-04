#include "..\Public\PhysX_Manager.h"
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


IMPLEMENT_SINGLETON(CPhysX_Manager)


CPhysX_Manager::CPhysX_Manager()
{

}


HRESULT CPhysX_Manager::Init()
{
	// 기초를 만든다.
	m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale());

	PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());		// 세상의 Desc을 채워준다.
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);				// 내 세상의 중력
	m_Dispatcher = PxDefaultCpuDispatcherCreate(4);				// Dispatcher : 아마 스레드 개수인 뜻
	if (!m_Dispatcher)
		return E_FAIL;
	sceneDesc.cpuDispatcher = m_Dispatcher;
	sceneDesc.filterShader = FilterShader;						// PxDefaultSimulationFilterShader;
	sceneDesc.simulationEventCallback = this;
	sceneDesc.kineKineFilteringMode = PxPairFilteringMode::eKEEP;
	sceneDesc.solverType = PxSolverType::eTGS;

	//PxCudaContextManagerDesc cuadContextManagerDesc;
	//m_pCudaContextManager = PxCreateCudaContextManager(*m_Foundation, cuadContextManagerDesc, PxGetProfilerCallback());
	//sceneDesc.cudaContextManager = m_pCudaContextManager;
	//sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;

	
	m_Scene = m_Physics->createScene(sceneDesc);				// 구조체대로 세상을 만든다.    
	
	m_Material = m_Physics->createMaterial(0.f, 0.f, 0.f);

	//// 바닥을 만든다.
	//PxRigidStatic* groundPlane = PxCreatePlane(*m_Physics, PxPlane(0, 1, 0, 0), *m_Material);
	//m_Scene->addActor(*groundPlane);



	return S_OK;
}

void CPhysX_Manager::Tick(_float fTimeDelta)
{
	if (1.f / 60.f < fTimeDelta)
		fTimeDelta = 1.f / 60.f;

	m_Scene->simulate(fTimeDelta);
	m_Scene->fetchResults(true);
	
}

PxRigidDynamic* CPhysX_Manager::Create_Box(_float3 vPos, _float3 vExtent, _uint iFlag)
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

PxRigidDynamic* CPhysX_Manager::Create_Sphere(_float3 vPos, _float fRad, _uint iFlag)
{
	// 박스를 만든다.
	PxShape* shape = m_Physics->createShape(PxSphereGeometry(PxReal(fRad)), *m_Material);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);

	shape->setSimulationFilterData(PxFilterData(iFlag, 0, 0, 0));


	PxTransform localTm(PxVec3(vPos.x, vPos.y, vPos.z)); // 포지션 설정
	PxRigidDynamic* body = m_Physics->createRigidDynamic(localTm);
	body->attachShape(*shape);
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	PxRigidBodyExt::updateMassAndInertia(*body, 0.1f); // 무게, 관성
	m_Scene->addActor(*body);
	shape->release();

	return body;
}

PxRigidDynamic * CPhysX_Manager::Create_PxBox(_float3 vExtent, _float fWeight, _float fAngleDump, PxMaterial* pMaterial, _float fMaxVel)
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

PxRigidDynamic * CPhysX_Manager::Create_PxSphere(_float3 vExtent, _float fWeight, _float fAngleDump, PxMaterial * pMaterial, _float fMaxVel)
{


	// 박스를 만든다.
	PxShape* shape = m_Physics->createShape(PxSphereGeometry(PxReal(vExtent.x)), *m_Material);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	shape->setDensityForFluid(10.f); // 밀도

	shape->setSimulationFilterData(PxFilterData(1, 0, 0, 0));

	// 이때의 vPos는 상대적인 포스
	// PxTransform localTm(PxVec3(vPos.x, vPos.y, vPos.z)); // 포지션 설정
	PxRigidDynamic* body = m_Physics->createRigidDynamic(PxTransform(0.f, 0.f, 0.f));
	body->attachShape(*shape);
	//body->setMassSpaceInertiaTensor(PxVec3(50.f, 100.f, 20.f));


	body->setAngularDamping(fAngleDump); // 회전을 방해하는 힘

	if (fMaxVel)
		body->setMaxLinearVelocity(fMaxVel); // 최대 속도

	PxRigidBodyExt::updateMassAndInertia(*body, fWeight); // 무게, 관성

	m_Scene->addActor(*body);

	shape->release();

	return body;
}

PxRigidDynamic * CPhysX_Manager::Create_JointBox(PxTransform vPos, _float3 vExtent, PxMaterial * pMaterial, _uint iFlag, _bool bKin)
{
	PxRigidDynamic* pDynamic = PxCreateDynamic(*m_Physics, vPos, PxSphereGeometry(vExtent.x), *pMaterial, 0.1f);

	PxShape* pShape;
	pDynamic->getShapes(&pShape, sizeof(PxShape));

	pDynamic->setSolverIterationCounts(10, 10);

	if (bKin)
		pDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	pShape->setSimulationFilterData(PxFilterData(iFlag, 0, 0, 0));

	return pDynamic;
}

// D6 joint with a spring maintaining its position
PxJoint* CPhysX_Manager::Create_DampedD6(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{


	//PxDistanceJoint* j = PxDistanceJointCreate(*m_Physics, a0, t0, a1, t1);
	//
	//j->setMinDistance(0.f);
	//j->setMaxDistance(0.1f);
	//j->setStiffness(0.1);
	//j->setDistanceJointFlag(PxDistanceJointFlag::eMIN_DISTANCE_ENABLED, true);
	//j->setDistanceJointFlag(PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);
	//j->setDistanceJointFlag(PxDistanceJointFlag::eSPRING_ENABLED, true);


	PxD6Joint* j = PxD6JointCreate(*m_Physics, a0, t0, a1, t1);
	j->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
	j->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);
	j->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);

	j->setDrive(PxD6Drive::eSLERP, PxD6JointDrive(6000, 10000, FLT_MAX, true));
	j->setDrive(PxD6Drive::eSWING, PxD6JointDrive(6000, 10000, FLT_MAX, true));
	j->setDrive(PxD6Drive::eTWIST, PxD6JointDrive(6000, 10000, FLT_MAX, true));


	//j->setMotion(PxD6Axis::eX, PxD6Motion::eLIMITED);
	//j->setMotion(PxD6Axis::eY, PxD6Motion::eLIMITED);
	//j->setMotion(PxD6Axis::eZ, PxD6Motion::eLIMITED);

	// j->setDistanceLimit(PxJointLinearLimit(0.01f, 0.001f));

	return j;
}

PxRigidStatic* CPhysX_Manager::Create_Static(_float3 vPos, _float3 vRot, _float3 vExtent, _uint iFlag)
{
	// 박스를 만든다.
	PxShape* shape = m_Physics->createShape(PxBoxGeometry(vExtent.x * 0.5f, vExtent.y * 0.5f, vExtent.z * 0.5f), *m_Material);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);

	shape->setSimulationFilterData(PxFilterData(iFlag, 0, 0, 0));

	_float4 vQuar; XMStoreFloat4(&vQuar, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&vRot)));
	PxTransform localTm(PxVec3(vPos.x, vPos.y, vPos.z), PxQuat(vQuar.x, vQuar.y, vQuar.z, vQuar.w)); // 포지션 설정
	PxRigidStatic* body = m_Physics->createRigidStatic(localTm);
	body->attachShape(*shape);
	// body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	// PxRigidBodyExt::updateMassAndInertia(*body, 10.f); // 무게, 관성
	m_Scene->addActor(*body);

	shape->release();

	return body;
}


void CPhysX_Manager::Add_Actor(PxActor * pAxtor)
{
	m_Scene->addActor(*pAxtor);
}

void CPhysX_Manager::Remove_Actor(PxActor* pAxtor)
{
	if (pAxtor)
		m_Scene->removeActor(*pAxtor);
}









void CPhysX_Manager::onConstraintBreak(PxConstraintInfo * constraints, PxU32 count)
{
}

void CPhysX_Manager::onWake(PxActor ** actors, PxU32 count)
{
}

void CPhysX_Manager::onSleep(PxActor ** actors, PxU32 count)
{
}

void CPhysX_Manager::onContact(const PxContactPairHeader & pairHeader, const PxContactPair * pairs, PxU32 nbPairs)
{


	if (!pairHeader.actors[0]->userData || !pairHeader.actors[1]->userData)
		return;


	CCollider* pColL = (CCollider*)pairHeader.actors[0]->userData;
	CCollider* pColR = (CCollider*)pairHeader.actors[1]->userData;

	if (!pColL->Get_IsCurTrigger() || !pColR->Get_IsCurTrigger())
		return;


	CGameObject* pObjL = pColL->Get_Owner();
	CGameObject* pObjR = pColR->Get_Owner();


	CCollider::OTHERTOMECOLDESC LDesc, RDesc;
	LDesc.pMyDesc = pColL->Get_Desc();
	LDesc.pOtherDesc = pColR->Get_Desc();
	LDesc.pOther = pObjR;

	RDesc.pMyDesc = pColR->Get_Desc();
	RDesc.pOtherDesc = pColL->Get_Desc();
	RDesc.pOther = pObjL;

	pObjL->OnCollision(LDesc);
	pObjR->OnCollision(RDesc);

} 

void CPhysX_Manager::onTrigger(PxTriggerPair * pairs, PxU32 count)
{
}

void CPhysX_Manager::onAdvance(const PxRigidBody * const * bodyBuffer, const PxTransform * poseBuffer, const PxU32 count)
{
}









void CPhysX_Manager::Free()
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





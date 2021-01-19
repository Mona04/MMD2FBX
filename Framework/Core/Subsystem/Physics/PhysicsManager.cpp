#include "Framework.h"
#include "PhysicsManager.h"

#include "Physics_Ect.h"

using namespace Framework;

PhysicsManager::PhysicsManager(Context* context) 
	: ISubsystem(context)
	, _col_config(nullptr), _dispatcher(nullptr), _broad_phase(nullptr)
	, _solver(nullptr), _world(nullptr)
{
	EventSystem::GetInstance().Subscribe(EventSystem::EventType::Init, EVENT_HANDLER(Init));
	EventSystem::GetInstance().Subscribe(EventSystem::EventType::Update, EVENT_HANDLER(Update));
}

PhysicsManager::~PhysicsManager()
{
	Clear();

	SAFE_DELETE(_debugDraw);
	SAFE_DELETE(_filterCB);

	SAFE_DELETE(_world);
	SAFE_DELETE(_solver);
	SAFE_DELETE(_broad_phase);
	SAFE_DELETE(_dispatcher);
	SAFE_DELETE(_col_config);
}

bool PhysicsManager::Init()
{
	_col_config = new btDefaultCollisionConfiguration();
	_dispatcher = new btCollisionDispatcher(_col_config);
	_broad_phase = new btDbvtBroadphase();
	_solver = new btSequentialImpulseConstraintSolver();
	_world = new btDiscreteDynamicsWorld(_dispatcher, _broad_phase, _solver, _col_config);
	
	_debugDraw = new PhysicsDebugDraw();
	_world->setDebugDrawer(_debugDraw);

	_filterCB = new PhysicsFilterCallback();
	_world->getPairCache()->setOverlapFilterCallback(_filterCB);

	_world->setGravity(btVector3(0, -9.8f * 10.0f, 0));

	btTransform groundTransform;
	groundTransform.setIdentity();

	auto ground_sp = new btStaticPlaneShape(btVector3(0, 1, 0), 0.0f);
	auto ground_ms = new btDefaultMotionState(groundTransform);
	auto rb_info = btRigidBody::btRigidBodyConstructionInfo(0, ground_ms, ground_sp, btVector3(0, 0, 0));
	auto ground_rb = new btRigidBody(rb_info);

	_world->addRigidBody(ground_rb); // remove 구현해야함
	_filterCB->_nonFilterProxy.push_back(ground_rb->getBroadphaseProxy());

	// ect init
	_time_before = _clock.getTimeMilliseconds();

	return true;
}

void PhysicsManager::Update()
{
	if (_use_physics)
	{
		_world->stepSimulation(_ms_per_tick * 0.001f, 10, 1/120.f);// in game
		//_world->stepSimulation(btScalar(cur - _time_before), 10, 1/120.f); // mmd simul
		_world->debugDrawWorld();
	}
}

void PhysicsManager::Clear()
{
}

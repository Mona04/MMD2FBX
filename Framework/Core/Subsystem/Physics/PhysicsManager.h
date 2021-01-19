#pragma once
#include <unordered_map>
#include "Framework/Core/Subsystem/ISubsystem.h"
#include "Util/Helper_BulletPhysics.h"

namespace Framework
{
	class PhysicsManager : public ISubsystem
	{
	public:
		PhysicsManager(class Context* context);
		virtual ~PhysicsManager();

		virtual bool Init() override;
		virtual void Update() override;
		void Clear();

	public:
		btDiscreteDynamicsWorld* GetWorld() { return _world; }
		
		void Set_MS_Per_Tick(float var) { _ms_per_tick = var; }
		void Set_Use_Physics(bool var) { _use_physics = var; }

	private:
		btDefaultCollisionConfiguration* _col_config;
		btCollisionDispatcher* _dispatcher;
		btDbvtBroadphase* _broad_phase;
		btSequentialImpulseConstraintSolver* _solver;
		btDiscreteDynamicsWorld* _world;

		btIDebugDraw* _debugDraw;
		class PhysicsFilterCallback* _filterCB;

		std::vector<class RigidBody*> _rigidBodys;

		btScalar _time_before;
		btClock _clock;

		float _ms_per_tick = 30;
		bool _use_physics = false;
	};
}
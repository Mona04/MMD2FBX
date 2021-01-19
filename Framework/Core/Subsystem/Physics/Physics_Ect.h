#pragma once
#include "Util/Helper_BulletPhysics.h"

namespace Framework
{
	class PhysicsDebugDraw : public btIDebugDraw
	{
	public:
		PhysicsDebugDraw() {}
		virtual ~PhysicsDebugDraw() {}

		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
		virtual void reportErrorWarning(const char* warningString) override;
		virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB,
			btScalar distance, int lifeTime, const btVector3& color) override {};
		virtual void draw3dText(const btVector3& location, const char* textString) override {};
		virtual void setDebugMode(int debugMode) override { }
		virtual int getDebugMode() const override { return DBG_DrawWireframe; }
	};

	struct PhysicsFilterCallback : public btOverlapFilterCallback
	{
		bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override
		{
			auto findIt = std::find_if(
				_nonFilterProxy.begin(),
				_nonFilterProxy.end(),
				[proxy0, proxy1](const auto& x) {return x == proxy0 || x == proxy1; }
			);
			if (findIt != _nonFilterProxy.end())
			{
				return true;
			}
			bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
			collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);
			return collides;
		}

		std::vector<btBroadphaseProxy*> _nonFilterProxy;
	};
}
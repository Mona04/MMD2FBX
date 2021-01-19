#pragma once
#include "IComponent.h"
#include "Util/Helper_BulletPhysics.h"

class btCollisionShape;
class btRigidBody;

namespace Framework
{
	class MotionState : public btMotionState
	{
	public:
		MotionState(class RigidBody* rb_pack, const Matrix& offset);

		void getWorldTransform(btTransform& worldTrans) const override;
		void setWorldTransform(const btTransform& worldTrans) override;

	private:
		RigidBody* _rb;
		Matrix _offset;
		Matrix _offset_inv;
	};

	// match with transform. Rigid Body Pack
	class RigidBody
	{
	public:
		RigidBody() : _physicsMgr(nullptr), _transform(nullptr)
			, _motionState(nullptr), _collisionShape(nullptr), _rigidBody(nullptr) {}
		~RigidBody() { Clear(); }

		void Create(const struct RBPack& rbPack, class Transform* trans);
		void Clear();

	public:
		class PhysicsManager* _physicsMgr;
		class Transform* _transform;  // transform match bone

		MotionState* _motionState;
		btCollisionShape* _collisionShape;
		btRigidBody* _rigidBody;
	};

	class RBJoint
	{
	public:
		RBJoint() : _physicsMgr(nullptr), _constraint(nullptr) {}
		~RBJoint() { Clear(); }
		void Create(const struct JointPack& jointPack, RigidBody* rb_A, RigidBody* rb_B);
		void Clear();

		class PhysicsManager* _physicsMgr;
		btGeneric6DofSpringConstraint* _constraint;
	};

	class RigidBodys : public IComponent
	{ 
	public:
		RigidBodys(class Context* context);
		virtual ~RigidBodys();

		void LoadFromFile(std::wstring_view path);
		void SaveToFile(std::wstring_view path);

		virtual bool Update() override;
		virtual void Clear() override;

		void SetRBInfo(std::wstring_view path);

	protected:
		class PhysicsManager* _physicsMgr;

		std::shared_ptr<RBInfo> _rbInfo;

		std::vector<std::unique_ptr<RigidBody>> _rbs;
		std::vector<std::unique_ptr<RBJoint>> _joints;
	};
}
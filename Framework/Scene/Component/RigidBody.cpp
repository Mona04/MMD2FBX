#include "Framework.h"
#include "RigidBody.h"

#include "Core/Subsystem/Physics/PhysicsManager.h"
#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"
#include "Resource/RBInfo.h"

#include <bullet/BulletCollision/CollisionShapes/btBoxShape.h>
#include <bullet/BulletCollision/CollisionShapes/btSphereShape.h>
#include <bullet/BulletCollision/CollisionShapes/btCapsuleShape.h>

using namespace Framework;
using namespace Helper_BulletPhysics;

//========================================================================
//  Motion State
//========================================================================

MotionState::MotionState(RigidBody* rb, const Matrix& offset)
	: _rb(rb), _offset(offset)
{
	_offset_inv = _offset.Inverse_RT();
}

void MotionState::getWorldTransform(btTransform& worldTrans) const
{
	if (_rb->_transform)
	{
		auto trans = _rb->_transform;
		Matrix m = (trans->GetWorldMatrix() * _offset);
		worldTrans.setOrigin(ToBtVec3(Matrix::MatrixToPosition(m)));
		worldTrans.setRotation(ToBtQuat(Matrix::MatrixToRotation(m)));
	}
	//rigidBody->Print();
	//rigidBody->SetHasSimulated(true);
}
void MotionState::setWorldTransform(const btTransform& worldTrans)
{
	if (_rb->_transform)
	{
		auto trans = _rb->_transform;
		Matrix m; worldTrans.getOpenGLMatrix((btScalar*)&m); 
		m = m.Transpose();
		m = m * _offset_inv;
		trans->Set_Physics(m);	
		// No Scale.  Scale should be implemented in collision shape
		//rigidBody->SetHasSimulated(true);
	}
}
//========================================================================

//========================================================================
//  Parts
//========================================================================
void RigidBody::Clear()
{
	if (_rigidBody)
	{
		if (_physicsMgr) _physicsMgr->GetWorld()->removeRigidBody(_rigidBody);
		SAFE_DELETE(_rigidBody);
	}
	SAFE_DELETE(_motionState);
	SAFE_DELETE(_collisionShape);
}
void RigidBody::Create(const RBPack& rb_pack, Transform * trans)
{
	Clear();
	if (trans)
	{
		_physicsMgr = trans->GetContext()->GetSubsystem<PhysicsManager>();
		_transform = trans;
	}
	else
	{
		LOG_ERROR("transform is not set");
		return;
	}

	auto size = Helper_BulletPhysics::ToBtVec3(rb_pack._size);
	switch (rb_pack._col_shape_type)
	{
	case Type_ColShape::Sphere:
		_collisionShape = new btSphereShape(size.x()); break;
	case Type_ColShape::Box:
		_collisionShape = new btBoxShape(size); break;
	case Type_ColShape::Capsule:
		_collisionShape = new btCapsuleShape(size.x(), size.y()); break;
	default:
		return;
	}
	if (!_collisionShape)
	{
		LOG_ERROR("Failed to create collision shape"); 
		Clear();
		return;
	}

	btScalar mass = rb_pack._mass;
	btVector3 localInertia;
	if (rb_pack._rb_type == Type_RB::Kinematic)
		mass = 0;
	if (mass != 0)
		_collisionShape->calculateLocalInertia(mass, localInertia);
	_collisionShape->setUserPointer(this);

	auto rot = Quaternion::QuaternionFromYawPitchRoll_ZYX(rb_pack._rot.z, rb_pack._rot.y, rb_pack._rot.x);
	auto pos = _transform->GetOffset() * Matrix::PositionToMatrix(rb_pack._pos);

	_motionState = new MotionState(this, pos * Matrix::QuaternionToMatrix(rot));

	btRigidBody::btRigidBodyConstructionInfo info(mass, _motionState, _collisionShape, localInertia);
	info.m_linearDamping = rb_pack._translateDimmer;
	info.m_angularDamping = rb_pack._rotateDimmer;
	info.m_restitution = rb_pack._repulsion;
	info.m_friction = rb_pack._friction;
	info.m_additionalDamping = true;

	_rigidBody = new btRigidBody(info);
	_rigidBody->setUserPointer(this);
	_rigidBody->setSleepingThresholds(0.01f, Math::ToRadian(0.1f));
	_rigidBody->setActivationState(DISABLE_DEACTIVATION);
	if (rb_pack._rb_type == Type_RB::Kinematic)
	{
		_rigidBody->setCollisionFlags(_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	}

	_physicsMgr->GetWorld()->addRigidBody(_rigidBody, 1 << rb_pack._group, rb_pack._group_mask);
}

void RBJoint::Create(const JointPack& jtPack, RigidBody* rb_A, RigidBody* rb_B)
{
	Clear();

	_physicsMgr = rb_A->_physicsMgr;
	if (!_physicsMgr)
	{
		LOG_ERROR("Invalid Rigid Body that have null physicsMgr");
		return;
	}

	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(ToBtVec3(jtPack._joint_trans));
	trans.getBasis().setEulerZYX(jtPack._joint_rot.x, jtPack._joint_rot.y, jtPack._joint_rot.z);

	btTransform invA = rb_A->_rigidBody->getWorldTransform().inverse();
	btTransform invB = rb_B->_rigidBody->getWorldTransform().inverse();
	invA = invA * trans;
	invB = invB * trans;

	_constraint = new btGeneric6DofSpringConstraint(
		*rb_A->_rigidBody,
		*rb_B->_rigidBody,
		invA, invB, true);

	_constraint->setLinearLowerLimit(ToBtVec3(jtPack._trans_low_limit));
	_constraint->setLinearUpperLimit(ToBtVec3(jtPack._trans_upper_limit));
	_constraint->setAngularLowerLimit(ToBtVec3(jtPack._rot_low_limit));
	_constraint->setAngularUpperLimit(ToBtVec3(jtPack._rot_upper_limit));

	if (jtPack._spring_trans_factor.x != 0)
	{
		_constraint->enableSpring(0, true);
		_constraint->setStiffness(0, jtPack._spring_trans_factor.x);
	}
	if (jtPack._spring_trans_factor.y != 0)
	{
		_constraint->enableSpring(1, true);
		_constraint->setStiffness(1, jtPack._spring_trans_factor.y);
	}
	if (jtPack._spring_trans_factor.z != 0)
	{
		_constraint->enableSpring(2, true);
		_constraint->setStiffness(2, jtPack._spring_trans_factor.z);
	}
	if (jtPack._spring_rot_factor.x != 0)
	{
		_constraint->enableSpring(3, true);
		_constraint->setStiffness(3, jtPack._spring_rot_factor.x);
	}
	if (jtPack._spring_rot_factor.y != 0)
	{
		_constraint->enableSpring(4, true);
		_constraint->setStiffness(4, jtPack._spring_rot_factor.y);
	}
	if (jtPack._spring_rot_factor.z != 0)
	{
		_constraint->enableSpring(5, true);
		_constraint->setStiffness(5, jtPack._spring_rot_factor.z);
	}

	_physicsMgr->GetWorld()->addConstraint(_constraint);
}

void RBJoint::Clear()
{
	if (_constraint)
	{
		if (_physicsMgr) _physicsMgr->GetWorld()->removeConstraint(_constraint);
		SAFE_DELETE(_constraint);
	}
}

//========================================================================



//========================================================================
//  Rigid Bodys
//========================================================================

RigidBodys::RigidBodys(Context* context)
	: IComponent(context), _physicsMgr(nullptr)
{
	typecode = TypeCode::RigidBodys;

	_physicsMgr = _context->GetSubsystem<PhysicsManager>();
	auto world = _physicsMgr->GetWorld();
}

RigidBodys::~RigidBodys()
{

}

void RigidBodys::LoadFromFile(std::wstring_view path)
{
}

void RigidBodys::SaveToFile(std::wstring_view path)
{
}


bool RigidBodys::Update()
{
	
	return true;
}

void RigidBodys::Clear()
{
}

void RigidBodys::SetRBInfo(std::wstring_view path)
{
	auto transform_array = _actor->GetComponent<Transform>()->Get_Transform_Array();
	_rbInfo = _context->GetSubsystem<ResourceManager>()->GetResource<RBInfo>(path);
	for (const auto& rb_pack : _rbInfo->Get_RBPacks())
	{
		auto trans = transform_array[rb_pack._bone_index];
		auto& rb = _rbs.emplace_back(std::make_unique<RigidBody>());
		rb->Create(rb_pack, trans);
	}
	for (const auto& jt_pack : _rbInfo->Get_JointPacks())
	{
		auto rb_A = _rbs[jt_pack._rb_index_A].get();
		auto rb_B = _rbs[jt_pack._rb_index_B].get();
		auto& joint = _joints.emplace_back(std::make_unique<RBJoint>());
		joint->Create(jt_pack, rb_A, rb_B);
	}
}



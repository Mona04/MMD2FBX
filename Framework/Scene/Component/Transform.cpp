#include "Framework.h"
#include "Transform.h"

#include "Resource/Skeletion.h"

using namespace Framework;

Transform::Transform(class Context* context)
	: IComponent(context)
	, _parent(nullptr), _append_index(-1), _append_weight(0)
	, _world(Matrix::identity), _world_before(Matrix::identity)
	, _local_position(), _local_scale(1), _local_rotation(0,0,0,1)
	, _anim_position(), _anim_scale(1), _anim_rotation(0,0,0,1), _ik_rotation(0,0,0,1)
	, _offset(Matrix::identity)
{
	typecode = TypeCode::Transform;
}

Transform::~Transform()
{
	Clear();
}

bool Transform::Update()
{
	UpdateTransform();

	return true;
}

void Transform::Clear()
{
	_parent = nullptr;
	_world = Matrix::identity;
	_world_before = Matrix::identity;

	_local_position = { 0 };
	_local_scale = { 1, 1, 1 };
	_local_rotation = { 0,0,0,1 };

	_anim_position = { 0 };
	_anim_scale = { 1, 1, 1 };
	_anim_rotation = { 0,0,0,1 };

	_offset = Matrix::identity;
	
	for (Transform* child : _childs)
		SAFE_DELETE(child);

	_skeleton = nullptr;
	_all_transform.clear();
	_all_transform.shrink_to_fit();
}

void Transform::SetLocalScale(const Vector3& vec)
{
	if (_local_scale == vec)
		return;

	_local_scale = vec;
	UpdateTransform();
}

void Transform::SetLocalRotation(const Quaternion& vec)
{
	if (_local_rotation == vec)
		return;

	_local_rotation = vec;
	UpdateTransform();
}

void Transform::SetLocalPosition(const Vector3& vec)
{
	if (_local_position == vec)
		return;

	_local_position = vec;
	UpdateTransform();
}

const Vector3 Transform::GetScale() const
{
	return Matrix::MatrixToScale(_world);
}

const Quaternion Transform::GetRotation() const
{
	return Matrix::MatrixToRotation(_world);
}

const Vector3 Transform::GetPosition() const
{
	return Matrix::MatrixToPosition(_world);
}

const Vector3 Transform::GetRotation_V() const
{
	Quaternion rot = GetRotation();
	return rot.ToEulerAngle();
}

void Transform::SetScale(const Vector3& vec)
{
	if (GetScale() == vec)
		return;

	if (HasParent())
	{
		Vector3 parentScale = _parent->GetScale();
		Vector3 scale;
		scale.x = vec.x / parentScale.x;
		scale.y = vec.y / parentScale.y;
		scale.z = vec.z / parentScale.z;

		SetLocalScale(scale);
	}
	else
		SetLocalScale(vec);
}

void Transform::SetRotation(const Quaternion& vec)
{
	if (GetRotation() == vec)
		return;
	
	if (HasParent())
	{
		// vec = child * parent; vec * parent_reverse = child;
		Quaternion r = vec * Quaternion::QuaternionToReverse(_parent->GetRotation());
		r.Normalize();

		SetLocalRotation(r);
	}
	else
		SetLocalRotation(vec);
}

void Transform::SetPosition(const Vector3& vec)
{
	if (GetPosition() == vec)
		return;

	if (HasParent())
		SetLocalPosition(vec - Matrix::MatrixToPosition(_parent->GetWorldMatrix()));
	else
		SetLocalPosition(vec);
}

void Transform::SetRotation_V(const Vector3& vec)
{
	SetRotation(vec.ToQuaternion());
}

void Transform::Translate(const Vector3& vec)
{
	auto rotation = Matrix::QuaternionToMatrix(GetRotation());
	SetPosition(GetPosition() + rotation * vec);
}

void Transform::Translate_Fixed_Axis(const Vector3& vec)
{
	SetPosition(GetPosition() + vec);
}

void Transform::Translate_Local(const Vector3& vec)
{
	SetLocalPosition(GetLocalPosition() + vec);
}

void Transform::Rotate_V(const Vector3& vec, bool bZ)
{
	auto result = vec + GetRotation_V();
	if (!bZ) result.z = 0;
	SetRotation_V(result);
}

void Transform::Rotate_Local_V(const Vector3& vec, bool bZ)
{
	auto result = vec + GetLocalRotation().ToEulerAngle();
	if (!bZ) result.z = 0;
	SetLocalRotation(result.ToQuaternion());
}

void Transform::SetParent(Transform* newParent)
{
	if (!newParent || _parent == newParent)
	{
		LOG_ERROR("Invalid patern");
		return;
	}

	if (newParent == this)
	{
		LOG_WARNING("parent is same with this");
		return;
	}

	_parent = newParent;
	_parent->AddChild(this);
}

void Transform::AddChild(Transform* child)
{
	if (!child)
	{
		LOG_ERROR("Null child");
		return;
	}
	for(Transform * m_child : _childs)
	{
		if (m_child == child)
		{
			LOG_WARNING("child already have included")
			return;
		}
	}


	if (child == this)
	{
		LOG_WARNING("child is same with this");
		return;
	}

	child->_parent = this;
	_childs.emplace_back(child);
}

Transform* Transform::AddChild()
{
	auto child = new Transform(_context);
	if (!child)
	{
		LOG_ERROR("Failed to create child");
		return nullptr;
	}

	child->_parent = this;
	_childs.emplace_back(child);

	return child;
}

void Transform::Set_Skeleton(std::wstring_view path, bool isMMD)
{
	_isMMD = isMMD;
	_skeleton = _resourceManager->GetResource<Skeleton>(path);
	if (!_skeleton)
		LOG_WARNING("Invalid new Skeleton. Check Path or File.")

	Construct_Transform_Array(); // tree 돌면서 얻는건 비효율적이기 때문에 array 로 미리 만듬.	
}

void Transform::Construct_Transform_Array()
{
	_all_transform.clear();
	_all_transform.shrink_to_fit();

	_all_transform.resize(_skeleton->GetBoneTreeNumber());
	Construct_Transform_Array_Recursive(_skeleton->GetRoot(), this, _all_transform);
	for (auto transform : _all_transform)
	{
		if(transform) transform->_all_transform = _all_transform;
	}
}

void Transform::Construct_Transform_Array_Recursive(Bone& bone, Transform* node, std::vector<Transform*>& all_trans)
{;
	if (bone.index >= 0)
	{
		all_trans[bone.index] = node;
		node->SetLocalPosition(Matrix::MatrixToPosition(bone.local));
		node->SetLocalScale(Matrix::MatrixToScale(bone.local));
		node->SetLocalRotation(Matrix::MatrixToRotation(bone.local));
		node->SetOffset(bone.offset);
		node->SetName(bone.name);
		node->_append_index = bone.append_index;
		node->_append_weight = bone.append_weight;
	}
	for(auto& bone : bone.childs)
		Construct_Transform_Array_Recursive(bone, node->AddChild(), _all_transform);
}

void Transform::UpdateTransform()
{
	_calced_local_rotation = _ik_rotation * _anim_rotation * _local_rotation;
	_calced_local_position = _local_position + _anim_position;

	if (_append_index != -1)
	{
		auto appended = _all_transform[_append_index];
		_calced_local_rotation = _calced_local_rotation * Math::Slerp(Quaternion(0, 0, 0, 1), appended->_calced_local_rotation, _append_weight);
		_calced_local_position = _calced_local_position + Math::Lerp(Vector3(0), appended->_anim_position, _append_weight);
	}
	_calced_local_rotation.Normalize();

	Matrix S, R, T;
	S = Matrix::ScaleToMatrix(_local_scale * _anim_scale);
	R = Matrix::QuaternionToMatrix(_calced_local_rotation);
	T = Matrix::PositionToMatrix(_calced_local_position);

	Matrix _local = T * R * S;

	if (HasParent())
		_world = _parent->GetWorldMatrix() * _local;
	else
		_world = _local;

	for (const auto& child : _childs)
		child->UpdateTransform();
}


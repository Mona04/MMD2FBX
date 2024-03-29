#pragma once
#include "IComponent.h"

namespace Framework
{
	class Transform : public IComponent
	{
	public:
		Transform(class Context* context);
		~Transform();

		virtual bool Update();
		virtual void Clear();

	public:
		//=============================================
		//Local
		const Vector3& GetLocalScale() const { return _local_scale; }
		const Quaternion& GetLocalRotation() const { return _local_rotation; }
		const Vector3& GetLocalPosition() const { return _local_position; }

		void SetLocalScale(const Vector3& vec);
		void SetLocalRotation(const Quaternion& vec);
		void SetLocalPosition(const Vector3& vec);

		const Quaternion& GetAnimRotation() const { return _anim_rotation; }
		const Vector3& GetAnimPosition() const { return _anim_position; }

		void SetAnimRotation(const Quaternion& vec) { _anim_rotation = vec; }
		void SetAnimPosition(const Vector3& vec) { _anim_position = vec; }
		void SetAnimPosition_FBX(const Vector3& vec) { _anim_position = vec - _local_position; }

		const Quaternion& GetIKRotation() const { return _ik_rotation; }
		void SetIKRotation(const Quaternion& vec) { _ik_rotation = vec; }

		const Quaternion& GetPhysicsRot() const { return _physics_rot; }
		const Vector3& GetPhysicsPos() const { return _physics_pos; }
		//=============================================

	public:
		//=============================================
		//World
		const Matrix& GetWorldMatrix() const { return _world; }
		const Matrix& GetWorldMatrix_Before() const { return _world_before; }
		void ResetWorldMatrix_Before() { _world_before = _world; }

		const Vector3 GetScale() const;
		const Quaternion GetRotation() const;
		const Vector3 GetPosition() const;
		const Vector3 GetRotation_V() const;

		void SetScale(const Vector3& vec);
		void SetRotation(const Quaternion& vec);
		void SetPosition(const Vector3& vec);
		void SetRotation_V(const Vector3& vec);

		void Translate(const Vector3& vec);
		void Translate_Fixed_Axis(const Vector3& vec);
		void Translate_Local(const Vector3& vec);
		void Rotate_V(const Vector3& vec, bool bZ = true);
		void Rotate_Local_V(const Vector3& vec, bool bZ = true);
		//======================================================

	public:
		//=====================================================
		// Physics
		Vector3 GetCalcLocalPos() { return _calced_local_pos; }
		Quaternion GetCalcLocalRot() { return _calced_local_rot; }
		void Set_Physics(const Matrix& m);
		//=====================================================

		//=============================================
		//Tree
		Transform* GetRoot() { return HasParent() ? GetParent()->GetRoot() : this; }
		Transform* GetParent() const { return _parent; }
		const std::vector<Transform*>& GetChilds() const { return _childs; }
		const uint GetChildCount() const { return static_cast<uint>(_childs.size()); }

		void SetParent(Transform* newParent);
		void AddChild(Transform* child);
		Transform* AddChild();

		const bool HasParent() const { return _parent ? true : false; }
		const bool HasChilds() const { return !_childs.empty(); }
		//=============================================

		//======================================================
		//Extra
		// inverse of parent2child. multipling with this makes global_position local_space_position; 
		const Matrix GetOffset() { return _offset; }
		void SetOffset(const Matrix& var) { _offset = var; }

		void Set_Skeleton(std::wstring_view path, bool isMMD = false);
		std::shared_ptr<class Skeleton> Get_Skeleton() { return _skeleton; }

		const std::vector<Transform*>& Get_Transform_Array() { return _all_transform; }

		void SetName(std::wstring_view name) { _name = name; }
		std::wstring GetName() { return _name; }

		int GetBoneIndex() { return _bone_index; }
		//======================================================
		
	protected:
		void Construct_Transform_Array();
		void Construct_Transform_Array_Recursive(struct Bone& bone, Transform* node, std::vector<Transform*>& all_trans);
		void UpdateTransform();

	protected:
		Matrix _world;         // 
		Matrix _world_before;   // 저장 후 불러오기 구현시 초반에 초기화 필요함

		Vector3 _local_scale;   // SRT 를 구현하기 때문에 m_local 이 없는것
		Quaternion _local_rotation;
		Vector3 _local_position;

		Quaternion _anim_rotation;
		Vector3 _anim_position;

		Quaternion _ik_rotation;

		Vector3 _physics_pos = 0;
		Quaternion _physics_rot = { 0,0,0,1 };

		Quaternion _calced_local_rot;  // appended 계산용
		Vector3 _calced_local_pos; 

		Matrix _offset;  // Not Tree Structured value, Must be multiplied once !!!!!

		Transform* _parent;
		std::vector<Transform*> _childs;

		int  _append_index; // another matrix index in m_all_transform that affect this matrix; 
		float _append_weight;  // 

		std::shared_ptr<Skeleton> _skeleton;
		std::vector<Transform*> _all_transform;	

		std::wstring _name;
		int _bone_index = -1;

		bool _isMMD = false;
	};
}
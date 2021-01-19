#pragma once
#include <vector>
#include <cstddef>
#include <memory>
#include "IResource.h"

namespace Framework
{
	// type of collision shape
	enum class Type_ColShape : unsigned int
	{
		Sphere = 0U,
		Box = 1U,
		Capsule = 2U,
	};

	// type of rigid body operation
	enum class Type_RB : unsigned int
	{
		Kinematic,
		Dynamic,
		Alligned
	};

	enum class Type_Joint : unsigned int
	{
		SpringDOF6,
		DOF6,
		P2P,
		ConeTwist,
		Slider,
		Hinge,
	};

	// match with transform. Rigid Body Pack
	struct RBPack
	{
		void SaveToFile(FileStream& stream) const;
		void LoadFromFile(FileStream& stream);

		std::wstring _name;
		uint _bone_index;
		int _group;
		int _group_mask;

		Vector3 _size;   // shape scale
		Vector3 _pos;
		Vector3 _rot;

		float _mass;
		float _translateDimmer;
		float _rotateDimmer;
		float _repulsion;
		float _friction;

		Type_ColShape _col_shape_type;
		Type_RB _rb_type;
	};

	struct JointPack
	{
		void SaveToFile(FileStream& stream) const;
		void LoadFromFile(FileStream& stream);

		std::wstring _name;
		Type_Joint _type_joint;

		int _rb_index_A;
		int _rb_index_B;

		Vector3 _joint_trans;
		Vector3 _joint_rot;
		Vector3 _trans_low_limit;
		Vector3 _trans_upper_limit;
		Vector3 _rot_low_limit;
		Vector3 _rot_upper_limit;
		Vector3 _spring_trans_factor;
		Vector3 _spring_rot_factor;
	};

	class RBInfo : public IResource, public std::enable_shared_from_this<RBInfo>
	{
	public:
		RBInfo(Context* context);
		virtual ~RBInfo();

		RBInfo(RBInfo& rhs) = delete;
		RBInfo(RBInfo&& rhs) = delete;
		RBInfo& operator=(RBInfo& rhs) = delete;
		RBInfo& operator=(RBInfo&& rhs) = delete;

		virtual bool LoadFromFile(std::wstring_view path) override;
		virtual bool SaveToFile(std::wstring_view path) const override;
		virtual void Clear() override;

	public:
		void Add_RBPack(const RBPack& pack) { _rb_packs.push_back(pack); }
		void Add_JointPack(const JointPack& pack) { _joint_packs.push_back(pack); }

		std::vector<RBPack> Get_RBPacks() { return _rb_packs; }
		std::vector<JointPack> Get_JointPacks() { return _joint_packs;}

	protected:
		std::vector<RBPack> _rb_packs;
		std::vector<JointPack> _joint_packs;
	};
}
#include "Framework.h"
#include "RBInfo.h"

using namespace Framework;

RBInfo::RBInfo(Context* context) : IResource(context)
{
	_typecode = TypeCode::RBInfo;

	Clear();
}

RBInfo::~RBInfo()
{
}

bool RBInfo::LoadFromFile(std::wstring_view path)
{
	Clear();

	FileStream stream;
	stream.Open(std::wstring(path), StreamMode::Read);
	{
		uint count = stream.ReadUInt();
		_rb_packs.resize(count);
		for (auto& v : _rb_packs)
			v.LoadFromFile(stream);

		count = stream.ReadUInt();
		_joint_packs.resize(count);
		for (auto& v : _joint_packs)
			v.LoadFromFile(stream);
	}
	stream.Close();
	return true;
}

bool RBInfo::SaveToFile(std::wstring_view path) const
{
	FileStream stream;
	stream.Open(std::wstring(path), StreamMode::Write);
	{
		stream.Write(static_cast<uint>(_rb_packs.size()));
		for (const auto& v : _rb_packs)
			v.SaveToFile(stream);

		stream.Write(static_cast<uint>(_joint_packs.size()));
		for (const auto& v : _joint_packs)
			v.SaveToFile(stream);
	}
	stream.Close();

	return true;
}

void RBInfo::Clear()
{
	_rb_packs.clear();
	_rb_packs.shrink_to_fit();
	_joint_packs.clear();
	_joint_packs.shrink_to_fit();
}

// ============================ Pack SaveLoad ==================================
#pragma region

void RBPack::SaveToFile(FileStream& stream) const
{
	stream.Write(_name);
	stream.Write(_bone_index);
	stream.Write(_group);
	stream.Write(_group_mask);
	stream.Write(_size);
	stream.Write(_pos);
	stream.Write(_rot);
	stream.Write(_mass);
	stream.Write(_translateDimmer);
	stream.Write(_rotateDimmer);
	stream.Write(_repulsion);
	stream.Write(_friction);
	stream.Write(static_cast<uint>(_col_shape_type));
	stream.Write(static_cast<uint>(_rb_type));
}

void RBPack::LoadFromFile(FileStream& stream)
{
	stream.Read(_name);
	stream.Read(_bone_index);
	stream.Read(_group);
	stream.Read(_group_mask);
	stream.Read(_size);
	stream.Read(_pos);
	stream.Read(_rot);
	stream.Read(_mass);
	stream.Read(_translateDimmer);
	stream.Read(_rotateDimmer);
	stream.Read(_repulsion);
	stream.Read(_friction);
	_col_shape_type = static_cast<Type_ColShape>(stream.ReadUInt());
	_rb_type = static_cast<Type_RB>(stream.ReadUInt());
}

void JointPack::SaveToFile(FileStream& stream) const
{
	stream.Write(_name);
	stream.Write(_rb_index_A);
	stream.Write(_rb_index_B);
	stream.Write(_joint_trans);
	stream.Write(_joint_rot);
	stream.Write(_trans_low_limit);
	stream.Write(_trans_upper_limit);
	stream.Write(_rot_low_limit);
	stream.Write(_rot_upper_limit);
	stream.Write(_spring_trans_factor);
	stream.Write(_spring_rot_factor);
	stream.Write(static_cast<uint>(_type_joint));
}

void JointPack::LoadFromFile(FileStream& stream)
{
	stream.Read(_name);
	stream.Read(_rb_index_A);
	stream.Read(_rb_index_B);
	stream.Read(_joint_trans);
	stream.Read(_joint_rot);
	stream.Read(_trans_low_limit);
	stream.Read(_trans_upper_limit);
	stream.Read(_rot_low_limit);
	stream.Read(_rot_upper_limit);
	stream.Read(_spring_trans_factor);
	stream.Read(_spring_rot_factor);
	_type_joint = static_cast<Type_Joint>(stream.ReadUInt());
}

#pragma endregion
#include "Framework.h"
#include "Skeletion.h"

using namespace Framework;

void IKLink::SaveToFile(FileStream& stream) const
{
	stream.Write(ikBoneIndex);
	stream.Write(enableAxisLimit);
	stream.Write(limitMin);
	stream.Write(limitMax);
}

void IKLink::LoadFromFile(FileStream& stream)
{
	stream.Read(ikBoneIndex);
	stream.Read(enableAxisLimit);
	stream.Read(limitMin);
	stream.Read(limitMax);
}


void Bone::SaveToFile(FileStream& stream) const
{
	stream.Write(name);

	stream.Write(index);
	stream.Write(parent_index);
	stream.Write(offset);
	stream.Write(local);
	stream.Write(static_cast<uint>(childs.size()));
	for (auto& child : childs)
		child.SaveToFile(stream);

	stream.Write(append_index);
	stream.Write(append_weight);

	stream.Write(ikTargetBone_index);
	stream.Write(ikIterationCount);
	stream.Write(ikItertationAngleLimit);
	stream.Write(static_cast<uint>(ikLinks.size()));
	for (auto& ikLink : ikLinks)
		ikLink.SaveToFile(stream);
}

void Bone::LoadFromFile(FileStream& stream)
{
	uint n_vector = 0;
	stream.Read(name);

	stream.Read(index);
	stream.Read(parent_index);
	stream.Read(offset);
	stream.Read(local);
	stream.Read(n_vector);
	childs.resize(n_vector);
	for (auto& child : childs)
		child.LoadFromFile(stream);

	stream.Read(append_index);
	stream.Read(append_weight);

	stream.Read(ikTargetBone_index);
	stream.Read(ikIterationCount);
	stream.Read(ikItertationAngleLimit);
	stream.Read(n_vector);
	ikLinks.resize(n_vector);
	for (auto& ikLink : ikLinks)
		ikLink.LoadFromFile(stream);
}


Skeleton::Skeleton(class Context* context) : IResource(context)
{
	_typecode = TypeCode::Skeleton;

	Clear();
}

Skeleton::~Skeleton()
{
}

bool Skeleton::LoadFromFile(std::wstring_view path)
{
	Clear();

	FileStream stream;
	stream.Open(std::wstring(path), StreamMode::Read);
	{
		uint size_bone_map;
		stream.Read(size_bone_map);

		int index;
		for (int i = 0 ; i < size_bone_map; i++)
		{
			stream.Read(index);
			_bone_map.emplace(std::make_pair(index, Bone()));
			GetBone(index).LoadFromFile(stream);
		}

		_root.LoadFromFile(stream);
		
		uint size_bone_links;
		stream.Read(size_bone_links);
		_bone_links.resize(size_bone_links);

		for (auto& link : _bone_links)
			link.LoadFromFile(stream);
	}
	return true;
}

bool Skeleton::SaveToFile(std::wstring_view path) const
{
	FileStream stream;
	stream.Open(std::wstring(path), StreamMode::Write);
	{
		uint size_bone_map = _bone_map.size();
		stream.Write(size_bone_map);
		for (auto iter = _bone_map.begin(); iter != _bone_map.end(); iter++)
		{
			stream.Write(iter->first);
			iter->second.SaveToFile(stream);
		}

		_root.SaveToFile(stream);

		uint size_bone_links = _bone_links.size();
		stream.Write(size_bone_links);
		for (auto& link : _bone_links)
			link.SaveToFile(stream);
	}
	return true;
}

void Skeleton::Clear()
{
	_none_bone = Bone(-1, Matrix::identity, None_StringW);
	_bone_map.clear();
}

Bone& Skeleton::GetBone(int index)
{
	auto result = _bone_map.find(index);
	if (result != _bone_map.end())
		return result->second;

	return _none_bone;
}

Bone& Skeleton::GetBone(const std::wstring& name)
{
	for (auto& iter : _bone_map)
	{
		if (iter.second.name == name)
			return iter.second;
	}
	return _none_bone;
}

Bone& Skeleton::AddBone(const Matrix& offset, const std::wstring& name)
{
	auto& finded = GetBone(name);
	if (finded.name == None_StringW)
	{
		int cur_index = _bone_map.size();
		_bone_map.emplace(std::make_pair(cur_index, Bone(cur_index, offset, name)));
		return _bone_map[cur_index];
	}
	else
		return finded;
}

Bone& Skeleton::AddBone(int index, const Matrix& offset, const std::wstring& name)
{
	auto& finded = GetBone(index);
	if (finded.name == None_StringW)
	{
		_bone_map.emplace(std::make_pair(index, Bone(_bone_map.size(), offset, name)));
		return _bone_map[index];
	}
	else
		return finded;
}

void CalcBoneNumber(Bone& bone, int& result)
{
	if (bone.name == None_StringW)
		return;

	result++;
	for (auto& bone : bone.childs)
		CalcBoneNumber(bone, result);
}

int Skeleton::GetBoneTreeNumber()
{
	int result = 0;
	CalcBoneNumber(_root, result);
	return result;
}

Bone& Skeleton::GetBoneInTree(int index, Bone& node)
{
	if (index < 0)
		return _root;

	if (node.index == index)
		return node;

	for (auto& child : node.childs)
	{
		auto& bone = GetBoneInTree(index, child);
		if (bone.index == index)
			return bone;
	}

	return _none_bone;
}
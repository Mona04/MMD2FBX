#pragma once
#include <vector>
#include <cstddef>
#include <memory>
#include "IResource.h"

namespace Framework
{
	struct IKLink
	{
		IKLink() : ikBoneIndex(-1), enableAxisLimit(0), limitMin(0), limitMax(0) {}
		void SaveToFile(FileStream& stream);
		void LoadFromFile(FileStream& stream);

		int			ikBoneIndex;
		unsigned char	enableAxisLimit;
		//m_enableLimit 이 1일 때
		Vector3	limitMin;	
		Vector3	limitMax;	
	};

	struct Bone
	{
		Bone() 
			: index(-1), parent_index(-1), name(None_StringW)
			, offset(Matrix::identity), local(Matrix::identity)
			, append_index(-1), append_weight(0)
			, ikTargetBone_index(-1), ikIterationCount(0), ikItertationAngleLimit(0)
		{}
		Bone(int index, Matrix offset, std::wstring_view name) 
			: index(index), parent_index(-1), name(name), offset(offset), local(Matrix::identity)
			, append_index(-1), append_weight(0)
			, ikTargetBone_index(-1), ikIterationCount(0), ikItertationAngleLimit(0)
		{}
		Bone(const Bone& rhs)
			: index(rhs.index), name(rhs.name), parent_index(rhs.parent_index)
			, offset(rhs.offset), local(rhs.local)
			, append_index(rhs.append_index), append_weight(rhs.append_weight)
			, ikTargetBone_index(-1), ikIterationCount(0), ikItertationAngleLimit(0)
		{
			childs.clear();
			ikLinks.clear();
			for (const auto& child : rhs.childs)
				childs.push_back(child);
			for (const auto& iklink : rhs.ikLinks)
				ikLinks.push_back(iklink);
		}

		Bone& AddChild() { return childs.emplace_back(Bone()); }
		Bone& AddChild(const Bone& bone) { return childs.emplace_back(bone); }
		IKLink& AddIKLink() { return ikLinks.emplace_back(IKLink()); }
		void SaveToFile(FileStream& stream);
		void LoadFromFile(FileStream& stream);

		std::wstring name;

		int index;
		int parent_index;
		Matrix offset;
		Matrix local;	
		std::vector<Bone> childs;

		int append_index;   // ik node 등 다른 노드의 위치에 직접적인 영향을 주기위한 인덱스
		float append_weight;

		int     ikTargetBone_index;
		int 	ikIterationCount;
		float	ikItertationAngleLimit;	// radian
		std::vector<IKLink> ikLinks;
	};

	class Skeleton : public IResource, public std::enable_shared_from_this<Skeleton>
	{
	public:
		Skeleton(Context* context);
		virtual ~Skeleton();

		Skeleton(Skeleton& rhs) = delete;
		Skeleton(Skeleton&& rhs) = delete;
		Skeleton& operator=(Skeleton& rhs) = delete;
		Skeleton& operator=(Skeleton&& rhs) = delete;

		virtual bool LoadFromFile(std::wstring_view path) override;
		virtual bool SaveToFile(std::wstring_view path) override;
		virtual void Clear() override;

	public:
		Bone& GetBone(int index);
		Bone& GetBone(const std::wstring& name);
		Bone& AddBone(const Matrix& offset, const std::wstring& name);
		Bone& AddBone(int index, const Matrix& offset, const std::wstring& name);
		std::unordered_map<int, Bone>& GetBoneMap() { return _bone_map; }
		int GetBoneTreeNumber();
		int GetBoneMapNumber() { return _bone_map.size(); }

		// fbx has just one root bone. this function is for root bone
		Bone& GetRoot() { return _root; }
		// mmd has many bone linked with just one bone. this function is for the head bone in link
		std::vector<Bone>& GetBoneLinks() { return _bone_links; }
		Bone& GetBoneInTree(int index, Bone& node);

	private:
		std::unordered_map<int, Bone> _bone_map;

		Bone _root; // fbx;
		std::vector<Bone> _bone_links; // mmd
		Bone _none_bone;
	};
}
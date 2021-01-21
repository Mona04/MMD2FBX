#include "Framework.h"
#include "MMD_Importer.h"
#include "MMD/CustomMMDHelper.h"

#include <algorithm>

#include "Framework/Core/DirectX/0_IADesc/Input_Desc.h"

#include "Framework/Core/Subsystem/Resource/ResourceManager.h"

#include "Framework/Resource/SkeletalMesh.h"
#include "framework/Resource/Skeletion.h"
#include "Framework/Resource/Morphs.h"

#include "Framework/Scene/Actor.h"
#include "Framework/Scene/Component/Transform.h"

using namespace Framework;
using namespace pmx;

bool MMD_Importer::LoadTransform(Transform* transform)
{
	auto mgr = _context->GetSubsystem<ResourceManager>();

	auto skeleton = std::make_shared<Skeleton>(_context);
	std::vector<std::pair<int, int>> bone_links_data;

	LoadSkeleton(skeleton, bone_links_data);

	LoadSkeleton_Resursive(skeleton, bone_links_data);
	mgr->RegisterResource<Skeleton>(skeleton, _basePathName + Extension_SkeletonW);
	transform->Set_Skeleton(skeleton->GetPath(), true);
	transform->Update();
	
	return true;
}

bool MMD_Importer::LoadSkeleton(std::shared_ptr<Skeleton> skeleton, std::vector<std::pair<int, int>>& bone_links_data)
{
	int bone_count = 0;
	_stream->read((char*)&bone_count, sizeof(int));
	std::wstring bone_name;
	std::wstring bone_english_name;

	for (int i = 0; i < bone_count; i++)
	{
		bone_name = std::move(ReadString(_stream, _setting.encoding));
		bone_english_name = std::move(ReadString(_stream, _setting.encoding));

		PMXBoneFlags bone_flag = PMXBoneFlags::AllowControl;
		int parent_index = -1;
		Vector3 pos = 0;
		Vector3 offset = 0;
		Matrix rotate = Matrix::identity;

		auto& bone = skeleton->AddBone(i, Matrix::identity, bone_name);

		_stream->read((char*)&pos, sizeof(float) * 3);
		PreProcess_MMD_Vector3(pos, true);
		bone.offset = Matrix::PositionToMatrix(pos).Inverse_RT();	// offset 은 T 기준이라서 얘가 맞음
		bone.parent_index = ReadIndex(_stream, _setting.bone_index_size);


		int target_index = -1;
		int level = 0;
		int key = 0;
		int ik_target_bone_index = 0;
		int ik_loop = 0;
		float ik_loop_angle_limit = 0;
		
		Vector3 lock_axis_orientation;
		Vector3 local_x_orientation;
		Vector3 local_y_orientation;	

		_stream->read((char*)&level, sizeof(int));
		_stream->read((char*)&bone_flag, sizeof(uint16_t));
		
		if (bone_flag & PMXBoneFlags::TargetShowMode) {   // 해결
			target_index = ReadIndex(_stream, _setting.bone_index_size);
		}
		else {   // 잘 모르겠음
			_stream->read((char*)&offset, sizeof(float) * 3);
			PreProcess_MMD_Vector3(offset);
		}

		if (bone_flag & (PMXBoneFlags::AppendRotate | PMXBoneFlags::AppendTranslate)) { 
			bone.append_index = ReadIndex(_stream, _setting.bone_index_size);
			_stream->read((char*)&bone.append_weight, sizeof(float));
		}
		
		if (bone_flag & PMXBoneFlags::FixedAxis) { // 허리 같은 거나 해당되고 값이 없음. 안쓰이는 듯.
			_stream->read((char*)&lock_axis_orientation, sizeof(float) * 3);
		}
		if (bone_flag & PMXBoneFlags::LocalAxis) { // axis orientation. 손가락만 해당되고 쓰면 이상해짐. 안쓰이는 듯
			_stream->read((char*)&local_x_orientation, sizeof(float) * 3);
			_stream->read((char*)&local_y_orientation, sizeof(float) * 3);
		}
		if (bone_flag & PMXBoneFlags::DeformOuterParent) {  // kyaru 에선 값이 없음. 안쓰이는 듯
			_stream->read((char*)&key, sizeof(int));
		}
		if (bone_flag & PMXBoneFlags::IK) {
			bone.ikTargetBone_index = ReadIndex(_stream, _setting.bone_index_size);
			_stream->read((char*)&bone.ikIterationCount, sizeof(int));
			_stream->read((char*)&bone.ikItertationAngleLimit, sizeof(float));

			int ik_link_count = 0;
			_stream->read((char*)&ik_link_count, sizeof(int));

			for (int i = 0; i < ik_link_count; i++) {
				auto& iklink = bone.AddIKLink();
				iklink.ikBoneIndex = ReadIndex(_stream, _setting.bone_index_size);
				
				_stream->read((char*)&iklink.enableAxisLimit, sizeof(uint8_t));
				if (iklink.enableAxisLimit == 1)
				{					
					_stream->read((char*)&iklink.limitMin, sizeof(float) * 3);
					_stream->read((char*)&iklink.limitMax, sizeof(float) * 3);
				}
			}
		}

		bone_links_data.push_back({ bone.parent_index, bone.index });
	}

	return true;
}

bool MMD_Importer::LoadSkeleton_Resursive(std::shared_ptr<class Skeleton> skeleton, std::vector<std::pair<int, int>>& bone_links_data)
{
	auto& root = skeleton->GetRoot();
	root = skeleton->GetBone(0);
	root.local = root.offset.Inverse_RT();

	std::sort(bone_links_data.begin(), bone_links_data.end(),
		[](std::pair<int, int> lhs, std::pair<int, int> rhs) {return lhs.first < rhs.first; });

	for (int i = 1; i < bone_links_data.size() ; i++)
	{
		Bone& parent = skeleton->GetBoneInTree(bone_links_data[i].first, root);
		Bone& current = skeleton->GetBone(bone_links_data[i].second);
		current.local = current.offset.Inverse_RT() * parent.offset;
		parent.AddChild(current);	
	}

	return true;
}
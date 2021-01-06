#include "Framework.h"
#include "MMD_Importer.h"
#include "MMD/CustomMMDHelper.h"

#include <algorithm>

#include "Core/DirectX/0_IADesc/Input_Desc.h"

#include "Core/Subsystem/Resource/ResourceManager.h"

#include "Resource/Mesh.h"
#include "Resource/SkeletalMesh.h"
#include "Resource/Skeletion.h"
#include "Resource/Material.h"
#include "Resource/Animation.h"
#include "Scene/Actor.h"
#include "Scene/Component/Renderable.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Animator.h"

using namespace Framework;
using namespace pmx;

bool MMD_Importer::LoadTransform(Transform* transform)
{
	auto mgr = _context->GetSubsystem<ResourceManager>();

	auto skeleton = std::make_shared<Skeleton>(_context);
	
	std::vector<std::pair<int, int>> bone_links_data;

	LoadSkeleton(skeleton, bone_links_data);

	LoadMorph(skeleton);

	LoadSkeleton_Resursive(skeleton, bone_links_data);
	mgr->RegisterResource<Skeleton>(skeleton, _basePathName + Extension_SkeletonW);
	transform->Set_Skeleton(skeleton->GetPath(), true);

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
		PreProcessing_MMD_Vector3(pos, true);
		bone.offset = Matrix::PositionToMatrix(pos).Inverse_SRT();	// offset 은 T 기준이라서 얘가 맞음
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
			PreProcessing_MMD_Vector3(offset);
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
	root.local = root.offset.Inverse_SRT();

	std::sort(bone_links_data.begin(), bone_links_data.end(),
		[](std::pair<int, int> lhs, std::pair<int, int> rhs) {return lhs.first < rhs.first; });

	for (int i = 1; i < bone_links_data.size() ; i++)
	{
		Bone& parent = skeleton->GetBoneInTree(bone_links_data[i].first, root);
		Bone& current = skeleton->GetBone(bone_links_data[i].second);
		current.local = current.offset.Inverse_SRT() * parent.offset;
		parent.AddChild(current);	
	}

	return true;
}

bool MMD_Importer::LoadMorph(std::shared_ptr<Skeleton> skeleton)
{
	int morph_count = 0;
	_stream->read((char*)&morph_count, sizeof(int));
	for (int i = 0; i < morph_count; i++)
	{
		std::wstring morph_name;
		std::wstring morph_english_name;
		MorphCategory category;
		MorphType morph_type;
		int offset_count;
		std::unique_ptr<PmxMorphVertexOffset[]> vertex_offsets;
		std::unique_ptr<PmxMorphUVOffset[]> uv_offsets;
		std::unique_ptr<PmxMorphBoneOffset[]> bone_offsets;
		std::unique_ptr<PmxMorphMaterialOffset[]> material_offsets;
		std::unique_ptr<PmxMorphGroupOffset[]> group_offsets;
		std::unique_ptr<PmxMorphFlipOffset[]> flip_offsets;
		std::unique_ptr<PmxMorphImpulseOffset[]> Impulse_offsets;

		morph_name = ReadString(_stream, _setting.encoding);
		morph_english_name = ReadString(_stream, _setting.encoding);
		_stream->read((char*)&category, sizeof(MorphCategory));
		_stream->read((char*)&morph_type, sizeof(MorphType));
		_stream->read((char*)&offset_count, sizeof(int));
		switch (morph_type)
		{
		case MorphType::Group:
			group_offsets = std::make_unique<PmxMorphGroupOffset[]>(offset_count);
			for (int i = 0; i < offset_count; i++)
			{
				group_offsets[i].Read(_stream, &_setting);
			}
			break;
		case MorphType::Vertex:
			vertex_offsets = std::make_unique<PmxMorphVertexOffset[]>(offset_count);
			for (int i = 0; i < offset_count; i++)
			{
				vertex_offsets[i].Read(_stream, &_setting);
			}
			break;
		case MorphType::Bone:
			bone_offsets = std::make_unique<PmxMorphBoneOffset[]>(offset_count);
			for (int i = 0; i < offset_count; i++)
			{
				bone_offsets[i].Read(_stream, &_setting);
			}
			break;
		case MorphType::Matrial:
			material_offsets = std::make_unique<PmxMorphMaterialOffset[]>(offset_count);
			for (int i = 0; i < offset_count; i++)
			{
				material_offsets[i].Read(_stream, &_setting);
			}
			break;
		case MorphType::UV:
		case MorphType::AdditionalUV1:
		case MorphType::AdditionalUV2:
		case MorphType::AdditionalUV3:
		case MorphType::AdditionalUV4:
			uv_offsets = std::make_unique<PmxMorphUVOffset[]>(offset_count);
			for (int i = 0; i < offset_count; i++)
			{
				uv_offsets[i].Read(_stream, &_setting);
			}
			break;
		default:
			throw;
		}
	}

	int frame_count = 0;
	_stream->read((char*)&frame_count, sizeof(int));
	for (int i = 0; i < frame_count; i++)
	{
		uint8_t frame_flag;
		int element_count;
		std::unique_ptr<PmxFrameElement[]> elements;

		std::wstring frame_name = ReadString(_stream, _setting.encoding);
		std::wstring frame_english_name = ReadString(_stream, _setting.encoding);
		_stream->read((char*)&frame_flag, sizeof(uint8_t));
		_stream->read((char*)&element_count, sizeof(int));
		//skeleton->AddBone(Matrix::identity, frame_name);
		elements = std::make_unique<PmxFrameElement[]>(element_count);
		for (int i = 0; i < element_count; i++)
		{
			uint8_t element_target;
			int index;
			_stream->read((char*)&element_target, sizeof(uint8_t));
			if (element_target == 0x00)
			{
				index = ReadIndex(_stream, _setting.bone_index_size);
				auto bone = skeleton->GetBone(index);
				int k = 0;
			}
			else {
				index = ReadIndex(_stream, _setting.morph_index_size);
			}
		}
	}

	int rigid_body_count = 0;
	_stream->read((char*)&rigid_body_count, sizeof(int));
	for (int i = 0; i < rigid_body_count; i++)
	{
		std::wstring rigid_body_name = ReadString(_stream, _setting.encoding);
		std::wstring rigid_body_english_name = ReadString(_stream, _setting.encoding);
		int target_bone;
		uint8_t group;
		uint16_t mask;
		uint8_t shape;
		float size[3];
		float position[3];
		float orientation[3];
		float mass;
		float move_attenuation;
		float rotation_attenuation;
		float repulsion;
		float friction;
		uint8_t physics_calc_type;
		target_bone = ReadIndex(_stream, _setting.bone_index_size);
		_stream->read((char*)&group, sizeof(uint8_t));
		_stream->read((char*)&mask, sizeof(uint16_t));
		_stream->read((char*)&shape, sizeof(uint8_t));
		_stream->read((char*)size, sizeof(float) * 3);
		_stream->read((char*)position, sizeof(float) * 3);
		_stream->read((char*)orientation, sizeof(float) * 3);
		_stream->read((char*)&mass, sizeof(float));
		_stream->read((char*)&move_attenuation, sizeof(float));
		_stream->read((char*)&rotation_attenuation, sizeof(float));
		_stream->read((char*)&repulsion, sizeof(float));
		_stream->read((char*)&friction, sizeof(float));
		_stream->read((char*)&physics_calc_type, sizeof(uint8_t));
	}

	int joint_count = 0;
	_stream->read((char*)&joint_count, sizeof(int));
	for (int i = 0; i < joint_count; i++)
	{
		std::wstring joint_name = ReadString(_stream, _setting.encoding);
		std::wstring joint_english_name = ReadString(_stream, _setting.encoding);
		PmxJointType joint_type;
		PmxJointParam param;
		_stream->read((char*)&joint_type, sizeof(uint8_t));

		int rigid_body1;
		int rigid_body2;
		float position[3];
		float orientaiton[3];
		float move_limitation_min[3];
		float move_limitation_max[3];
		float rotation_limitation_min[3];
		float rotation_limitation_max[3];
		float spring_move_coefficient[3];
		float spring_rotation_coefficient[3];

		rigid_body1 = ReadIndex(_stream, _setting.rigidbody_index_size);
		rigid_body2 = ReadIndex(_stream, _setting.rigidbody_index_size);
		_stream->read((char*)position, sizeof(float) * 3);
		_stream->read((char*)orientaiton, sizeof(float) * 3);
		_stream->read((char*)move_limitation_min, sizeof(float) * 3);
		_stream->read((char*)move_limitation_max, sizeof(float) * 3);
		_stream->read((char*)rotation_limitation_min, sizeof(float) * 3);
		_stream->read((char*)rotation_limitation_max, sizeof(float) * 3);
		_stream->read((char*)spring_move_coefficient, sizeof(float) * 3);
		_stream->read((char*)spring_rotation_coefficient, sizeof(float) * 3);
	}

	// soft body
	if (_version == 2.1f)
	{
		int soft_body_count = 0;
		_stream->read((char*) &soft_body_count, sizeof(int));;
		//for (int i = 0; i < soft_body_count; i++)
		//{
		//	this->soft_bodies[i].Read(stream, &setting);
		//}
	}
	return false;
}
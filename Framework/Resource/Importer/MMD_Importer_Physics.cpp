#include "Framework.h"
#include "MMD_Importer.h"
#include "MMD/CustomMMDHelper.h"

#include <algorithm>

#include "Framework/Core/DirectX/0_IADesc/Input_Desc.h"

#include "Framework/Core/Subsystem/Resource/ResourceManager.h"


#include "framework/Resource/RBInfo.h"

#include "Framework/Scene/Actor.h"
#include "Framework/Scene/Component/Transform.h"
#include "Framework/Scene/Component/RigidBody.h"

using namespace Framework;
using namespace pmx;

bool MMD_Importer::LoadPhysics(RigidBodys* rigidBodys)
{
	auto mgr = _context->GetSubsystem<ResourceManager>();
	auto rbInfo = std::make_shared<RBInfo>(_context);

	LoadRigidBody(rbInfo);
	LoadRBJoint(rbInfo);

	mgr->RegisterResource<RBInfo>(rbInfo, _basePathName + Extension_RBInfoW);
	rigidBodys->SetRBInfo(rbInfo->GetPath());

	// soft body
	if (_version == 2.1f)
	{
		int soft_body_count = 0;
		_stream->read((char*)&soft_body_count, sizeof(int));;
		//for (int i = 0; i < soft_body_count; i++)
		//{
		//	this->soft_bodies[i].Read(stream, &setting);
		//}
	}

	return true;
}

bool MMD_Importer::LoadRigidBody(std::shared_ptr<class RBInfo> rbInfo)
{
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

		elements = std::make_unique<PmxFrameElement[]>(element_count);
		for (int i = 0; i < element_count; i++)
		{
			uint8_t element_target;
			int index;
			_stream->read((char*)&element_target, sizeof(uint8_t));
			if (element_target == 0x00)
			{
				index = ReadIndex(_stream, _setting.bone_index_size);
			}
			else {
				index = ReadIndex(_stream, _setting.morph_index_size);
			}
		}
	}

	int rigid_body_count = 0;
	_stream->read((char*)&rigid_body_count, sizeof(int));
	
	std::wstring rigid_body_english_name;
	int target_bone;  // root node ¾øÀ½
	uint8_t group;
	uint16_t mask;
	uint8_t shape;   // Sphere : 0, Box : 1, Capsule : 2
	uint8_t physics_calc_type; // static : 0, dynamic : 1, dynamic&boneMerge : 2

	for (int i = 0; i < rigid_body_count; i++)
	{
		RBPack rb;

		rb._name = ReadString(_stream, _setting.encoding);
		rigid_body_english_name = ReadString(_stream, _setting.encoding);
		rb._bone_index = ReadIndex(_stream, _setting.bone_index_size);
		_stream->read((char*)&group, sizeof(uint8_t));
		_stream->read((char*)&mask, sizeof(uint16_t));
		_stream->read((char*)&shape, sizeof(uint8_t));
		_stream->read((char*)&rb._size, sizeof(float) * 3);
		_stream->read((char*)&rb._pos, sizeof(float) * 3);
		_stream->read((char*)&rb._rot, sizeof(float) * 3); // euler z->y->x
		_stream->read((char*)&rb._mass, sizeof(float));
		_stream->read((char*)&rb._translateDimmer, sizeof(float));
		_stream->read((char*)&rb._rotateDimmer, sizeof(float));
		_stream->read((char*)&rb._repulsion, sizeof(float));
		_stream->read((char*)&rb._friction, sizeof(float));
		_stream->read((char*)&physics_calc_type, sizeof(uint8_t));

		PreProcess_MMD_Vector3(rb._size, true);
		PreProcess_MMD_Vector3(rb._pos, true);

		rb._group = group;
		rb._group_mask = mask;
		rb._col_shape_type = (Type_ColShape)shape;
		rb._rb_type = (Type_RB)physics_calc_type;

		rbInfo->Add_RBPack(rb);
	}

	return true;
}

bool MMD_Importer::LoadRBJoint(std::shared_ptr<class RBInfo> rbInfo)
{
	int joint_count = 0;
	_stream->read((char*)&joint_count, sizeof(int));
	
	std::wstring joint_english_name;
	PmxJointType joint_type;

	for (int i = 0; i < joint_count; i++)
	{
		JointPack joint;

		joint._name = ReadString(_stream, _setting.encoding);
		joint_english_name = ReadString(_stream, _setting.encoding);

		_stream->read((char*)&joint_type, sizeof(uint8_t));

		joint._rb_index_A = ReadIndex(_stream, _setting.rigidbody_index_size);
		joint._rb_index_B = ReadIndex(_stream, _setting.rigidbody_index_size);

		_stream->read((char*)&joint._joint_trans, sizeof(float) * 3);
		_stream->read((char*)&joint._joint_rot, sizeof(float) * 3);
		_stream->read((char*)&joint._trans_low_limit, sizeof(float) * 3);
		_stream->read((char*)&joint._trans_upper_limit, sizeof(float) * 3);
		_stream->read((char*)&joint._rot_low_limit, sizeof(float) * 3);
		_stream->read((char*)&joint._rot_upper_limit, sizeof(float) * 3);
		_stream->read((char*)&joint._spring_trans_factor, sizeof(float) * 3);
		_stream->read((char*)&joint._spring_rot_factor, sizeof(float) * 3);
		
		PreProcess_MMD_Vector3(joint._joint_trans, true);
		PreProcess_MMD_Vector3(joint._trans_low_limit, true);
		PreProcess_MMD_Vector3(joint._trans_upper_limit, true);
		PreProcess_MMD_Vector3(joint._spring_trans_factor, true);

		joint._type_joint = (Type_Joint)joint_type;

		rbInfo->Add_JointPack(joint);
	}
	
	return true;
}

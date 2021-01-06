#include "Framework.h"
#include "MMD_Importer.h"
#include "MMD/CustomMMDHelper.h"
#include <unordered_map>

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

bool MMD_Importer::Load_Animation(std::wstring_view path, Actor* actor, Context* context)
{
	if (FileSystem::GetFileExtensionFromPath(path) != L".vmd")
	{
		LOG_ERROR("Invalid Form");
		return false;
	}
	if (FileSystem::IsExistFile(path) == false)
	{
		LOG_ERROR("NO File");
		return false;
	}

	_context = context;
	auto mgr = context->GetSubsystem<ResourceManager>();

	Init_VMD(path);

	auto transform = actor->GetComponent<Transform>();
	if (!transform)
		transform = actor->AddComponent<Transform>();
	auto animator = actor->GetComponent<Animator>();
	if (!animator)
		animator = actor->AddComponent<Animator>();

	auto animation = std::make_shared<Animation>(_context);
	auto skeleton = transform->Get_Skeleton();
	if (!skeleton)
	{
		LOG_ERROR("No Skeleton available.");
		return false;
	}

	LoadBoneFrame(animation, skeleton);
	LoadFaceFrame(animation, skeleton);
	LoadCameraFrame(animation, skeleton);
	LoadLightFrame(animation, skeleton);
	LoadIKFrame(animation, skeleton);

	if (_stream->peek() != std::ios::traits_type::eof())
	{
		std::cerr << "vmd stream has unknown data." << std::endl;
		return false;
	}

	mgr->RegisterResource(animation, _basePathName + Extension_AnimationW);
	animator->SetAnimation(animation->GetPath());

	return true;
}


bool MMD_Importer::Init_VMD(std::wstring_view path)
{
	_basePath = FileSystem::GetFileDirectoryFromPath(path);
	_basePathName = FileSystem::GetFileDirectoryFromPath(path) + FileSystem::GetIntactFileNameFromPath(path);

	_fb = new std::filebuf();
	if (!_fb->open(path.data(), std::ios::in | std::ios::binary))
	{
		LOG_WARNING("Can't open the " + FileSystem::ToString(path) + ", please check");
		return false;
	}

	_stream = new std::istream(_fb);
	if (!_stream)
	{
		LOG_WARNING("Failed to create IStream");
		return false;
	}

	auto magic = ReadPmxString(_stream, 30);
	auto name = ReadPmxString(_stream, 20);
	if (strncmp(magic.c_str(), "Vocaloid Motion Data", 20))
	{
		std::cerr << "invalid vmd file." << std::endl;
		return false;
	}

	int version = std::atoi(magic.data() + 20);
}

bool MMD_Importer::LoadBoneFrame(std::shared_ptr<Animation> animation, std::shared_ptr<Skeleton> skeleton)
{
	int bone_frame_num;
	_stream->read((char*)&bone_frame_num, sizeof(int));
	animation->Init_Channels(skeleton->GetBoneMap().size());
	animation->Set_MsPerTic(1000.f / 33.333333f);  // 30 fps 
	animation->Set_IsMMD(true);
	animation->Set_IsLoop(false);
	animation->Set_UseIK(true);

	std::wstring name;
	int frame = 0;
	for (int i = 0; i < bone_frame_num; i++)
	{
		Vector3 pos;
		Quaternion rot;
		uint8_t interpolation[4][16];  // x,y,z,rot bezier. 1byte 만 유효한 값임

		name = ReadPmxWString(_stream, 15);
		_stream->read((char*)&frame, sizeof(int));
		_stream->read((char*)&pos, sizeof(float) * 3); PreProcessing_MMD_Vector3(pos, true);
		_stream->read((char*)&rot, sizeof(float) * 4); rot.Normalize();
		_stream->read((char*)interpolation, 4 * 4 * 4); 

		auto& bone = skeleton->GetBone(name);
		if (bone.index >= 0)
		{
			auto& channel = animation->Get_Channel(bone.index);
			auto& key = channel.Add_Key();
			key.pos = pos;
			key.rot = rot; // Vector3(rot.x, rot.y, rot.z).ToQuaternion();
			key.frame = frame;
			CalcBezier(key.bezier_x,   interpolation[0]);
			CalcBezier(key.bezier_y,   interpolation[1]);
			CalcBezier(key.bezier_z,   interpolation[2]);
			CalcBezier(key.bezier_rot, interpolation[3]);
			
			animation->Set_Duration(frame);
		}
	}

	return true;
}

void MMD_Importer::CalcBezier(Vector2* desc, const uint8_t* src)
{
	int x0 = src[0]; int y0 = src[4]; 
	int x1 = src[8]; int y1 = src[12];
	desc[0] = Vector2((float)x0 / 127.0f, (float)y0 / 127.0f);
	desc[1] = Vector2((float)x1 / 127.0f, (float)y1 / 127.0f);
}

bool MMD_Importer::LoadFaceFrame(std::shared_ptr<class Animation> animation, std::shared_ptr<class Skeleton> skeleton)
{
	// face frames
	int face_frame_num;
	_stream->read((char*)&face_frame_num, sizeof(int));
	
	std::wstring name;
	int frame = 0;

	for (int i = 0; i < face_frame_num; i++)
	{
		float weight;
		name = ReadPmxWString(_stream, 15);  // bone 에 없으니까 알아두셈
		_stream->read((char*)&frame, sizeof(int));
		_stream->read((char*)&weight, sizeof(float));
	}
	return false;
}

bool MMD_Importer::LoadCameraFrame(std::shared_ptr<class Animation> animation, std::shared_ptr<class Skeleton> skeleton)
{
	// camera frames
	int camera_frame_num;
	_stream->read((char*)&camera_frame_num, sizeof(int));

	std::wstring name;
	int frame = 0;

	for (int i = 0; i < camera_frame_num; i++)
	{
		float distance;
		float position[3];
		float orientation[3];
		char interpolation[6][4];
		float angle;
		char unknown[3];
		_stream->read((char*)&frame, sizeof(int));
		_stream->read((char*)&distance, sizeof(float));
		_stream->read((char*)position, sizeof(float) * 3);
		_stream->read((char*)orientation, sizeof(float) * 3);
		_stream->read((char*)interpolation, sizeof(char) * 24);
		_stream->read((char*)&angle, sizeof(float));
		_stream->read((char*)unknown, sizeof(char) * 3);
	}

	return true;
}

bool MMD_Importer::LoadLightFrame(std::shared_ptr<class Animation> animation, std::shared_ptr<class Skeleton> skeleton)
{
	// light frames
	int light_frame_num;
	_stream->read((char*)&light_frame_num, sizeof(int));
	
	std::wstring name;
	int frame = 0;

	for (int i = 0; i < light_frame_num; i++)
	{
		float color[3];
		float position[3];
		_stream->read((char*)&frame, sizeof(int));
		_stream->read((char*)color, sizeof(float) * 3);
		_stream->read((char*)position, sizeof(float) * 3);
	}

	return false;
}

bool MMD_Importer::LoadIKFrame(std::shared_ptr<class Animation> animation, std::shared_ptr<class Skeleton> skeleton)
{
	std::wstring name;
	int frame = 0;

	// unknown2
	_stream->read((char*)&frame, 4);

	// ik frames
	if (_stream->peek() != std::ios::traits_type::eof())
	{
		int ik_num;
		_stream->read((char*)&ik_num, sizeof(int));
		for (int i = 0; i < ik_num; i++)
		{
			bool display;
			std::vector<VmdIkEnable> ik_enable;
			_stream->read((char*)&frame, sizeof(int));
			_stream->read((char*)&display, sizeof(uint8_t));
			int ik_count;
			_stream->read((char*)&ik_count, sizeof(int));
			ik_enable.resize(ik_count);
			for (int i = 0; i < ik_count; i++)
			{
				ik_enable[i].ik_name = ReadPmxWString(_stream, 20);
				_stream->read((char*)&ik_enable[i].enable, sizeof(uint8_t));
			}
		}
	}
	return true;
}


#include "Framework.h"
#include "VMD2Binary.h"

#include "Scene/Actor.h"
#include "Scene/Component/Renderable.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Animator.h"
#include "Resource/Animation.h"
#include "Resource/Skeletion.h"
#include "Core/Subsystem/Resource/ResourceManager.h"

using namespace Framework;


VMD2Binary::VMD2Binary()
{
}

VMD2Binary::~VMD2Binary()
{
}

bool VMD2Binary::Init(Actor* actor, int frame_start, int frame_end)
{
	_actor = actor;
	_start_frame = frame_start;
	_end_frame = frame_end;

	auto src_animation = _actor->GetComponent<Animator>()->GetAnimation();
	_ms_per_tick = src_animation->Get_MsPerTic();
	auto _num_src_bone = actor->GetComponent<Transform>()->Get_Transform_Array().size();
	_channels_data = std::vector<Bone_Channel>(_num_src_bone);

	if (frame_start == 0 && frame_end == -1)
	{
		for (auto i = 0; i < src_animation->Get_Channels().size(); i++)
		{
			const auto& src_channel = src_animation->Get_Channels()[i];
			auto& dst_channel = _channels_data[i];

			dst_channel.keys.resize(src_channel.keys.size());
			std::copy(src_channel.keys.begin(), src_channel.keys.end(), dst_channel.keys.data());
		}

		for (const auto& src_morph : src_animation->Get_Morph_Channels())
		{
			auto& dst_channel = _morph_channels_data[src_morph.first];

			dst_channel.keys.resize(src_morph.second.keys.size());
			std::copy(src_morph.second.keys.begin(), src_morph.second.keys.end(), dst_channel.keys.data());
		}
	}
	else
	{
		for (auto i = 0; i < src_animation->Get_Channels().size(); i++)
		{
			const auto& src_channel = src_animation->Get_Channels()[i];
			auto& dst_channel = _channels_data[i];
			for (auto j = 0; j < src_channel.keys.size(); j++)
			{
				const auto& key = src_channel.keys[j];
				if (key.frame >= frame_start && key.frame <= frame_end)
				{
					const auto& src_key = src_channel.keys[j];
					auto& key = dst_channel.Add_Key();
					memcpy(&key, &src_key, sizeof(key));
					key.frame -= frame_start;

					if (_end_frame < key.frame) // check endframe
						_end_frame = key.frame;
				}
			}
		}
		for (const auto& src_morph : src_animation->Get_Morph_Channels())
		{
			const auto& src_channel = src_morph.second;
			auto& dst_channel = _morph_channels_data[src_morph.first];

			for (auto j = 0; j < src_channel.keys.size(); j++)
			{
				const auto& key = src_channel.keys[j];
				if (key.frame >= frame_start && key.frame <= frame_end)
				{
					const auto& src_key = src_channel.keys[j];
					auto& key = dst_channel.Add_Key();
					memcpy(&key, &src_key, sizeof(key));
					key.frame -= frame_start;

					if (_end_frame < key.frame) // check endframe
						_end_frame = key.frame;
				}
			}
		}
	}

	return true;
}

void VMD2Binary::Clear()
{
	_channels_data.clear();
	_channels_data.shrink_to_fit();
}

bool VMD2Binary::Record_Animation_Frame(int cur_tik)
{
	const auto& boneMap = _actor->GetComponent<Transform>()->Get_Skeleton()->GetBoneMap();
	const auto& transform_array = _actor->GetComponent<Transform>()->Get_Transform_Array();
	for (auto iter = boneMap.begin(); iter != boneMap.end(); iter++)
	{
		std::vector<int> indices;

		for (const auto& ikchain : iter->second.ikLinks)
		{
			auto target_index = ikchain.ikBoneIndex;
			if (target_index != -1)
			{
				if (cur_tik == 0)
					_channels_data[target_index].keys.clear();

				auto& key = _channels_data[target_index].Add_Key();
				const auto& trans = transform_array[target_index];
				key.pos = trans->GetAnimPosition();
				key.rot = trans->GetIKRotation() * trans->GetAnimRotation();
				key.scale = Vector3(1, 1, 1);
				key.frame = cur_tik;
			}
		}
	}

	return true;
}

bool VMD2Binary::Export(const std::wstring_view path, Context* context)
{
	if (_actor == nullptr)
	{
		LOG_ERROR("NO Actor");
		return false;
	}

	auto animation = std::make_shared<Animation>(context);
	auto src_animation = _actor->GetComponent<Animator>()->GetAnimation();

	animation->Init_Channels(_channels_data.size());
	animation->Set_MsPerTic(src_animation->Get_MsPerTic());
	animation->Set_IsLoop(false);
	animation->Set_IsMMD(true);
	animation->Set_UseIK(false);
	if (_end_frame < 0)
		animation->Set_Duration(src_animation->Get_Duration() - _start_frame);
	else
	{
		animation->Set_Duration(_end_frame + 1 - _start_frame);
		for (auto i = 0; i < _channels_data.size(); i++)   // append last key for interpolation
		{
			auto& dst_channel = _channels_data[i];

			if (dst_channel.keys.size() > 0)
			{
				auto& end_key = dst_channel.Add_Key();
				const auto& last_key = dst_channel.keys[dst_channel.keys.size() - 2];				
				memcpy(&end_key, &last_key, sizeof(end_key));
				end_key.frame = _end_frame + 1;
			}
		}
	}

	for (auto i = 0; i < _channels_data.size(); i++)
	{
		const auto& channel_data = _channels_data[i];
		auto& dst_channel = animation->Get_Channels()[i];

		dst_channel.keys.resize(channel_data.keys.size());
		std::copy(channel_data.keys.begin(), channel_data.keys.end(), dst_channel.keys.data());
	}
	for (const auto& src_morph : _morph_channels_data)
	{
		auto& dst_channel = animation->Get_Morph_Channels()[src_morph.first];

		dst_channel.keys.resize(src_morph.second.keys.size());
		std::copy(src_morph.second.keys.begin(), src_morph.second.keys.end(), dst_channel.keys.data());
	}

	auto dst_path = FileSystem::GetIntactFileNameFromPath(path) + Extension_AnimationW;
	context->GetSubsystem<ResourceManager>()->RegisterResource(animation, dst_path);

	return true;
}

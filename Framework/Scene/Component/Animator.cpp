#include "Framework.h"
#include "Animator.h"

#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"
#include "Resource/Animation.h"

#include "Core/Subsystem/Resource/ResourceManager.h"
#include "Core/Subsystem/Function/Timer.h"

using namespace Framework;

Animator::Animator(Context* context)
	: IComponent(context),
	_timer(nullptr), _root(nullptr),
	_current_accumulated_delta_time_ms(0)
{
	typecode = TypeCode::Animator;
	_timer = _context->GetSubsystem<Timer>();
}

Animator::~Animator()
{
	Clear();
}

bool Animator::Update()
{
	_isPlaying = true;
	if (_isPlaying)
	{
		_current_accumulated_delta_time_ms += _ms_per_tick;// _timer->GetDeltaTimeMs();

		while (_animation->Get_MsPerTic() <= _current_accumulated_delta_time_ms)
		{
			if (_current_accumulated_delta_time_ms > 10000)
				_current_accumulated_delta_time_ms = (int)_current_accumulated_delta_time_ms % (int)_animation->Get_MsPerTic();

			_current_accumulated_delta_time_ms -= _animation->Get_MsPerTic();
			_current_frame += 1;
			if (_current_frame >= _animation->Get_Duration())
			{
				if (_animation->IsLoop())
				{
					_current_frame = 1; // mixamo 는 처음과 끝이 같은 프레임이라 1로 해야함
					ZeroMemory(_current_key_index_map.data(), _current_key_index_map.size() * sizeof(uint));
				}
				else
				{
					_isEnd = true;
					_current_frame = _animation->Get_Duration() - 1;
				}
			}
		}

		Animate();
	}
	return true;
}

void Animator::Clear()
{
	_root = nullptr;
	_current_frame = 0;
	_current_accumulated_delta_time_ms = 0;
}

void Animator::SetAnimation(std::wstring_view path)
{
	Clear();

	auto mgr = _context->GetSubsystem<ResourceManager>();

	_animation = mgr->GetResource<Animation>(path);
	if (_root == nullptr) _root = _actor->GetComponent<Transform>();

	_current_key_index_map.resize(_animation->Get_Channels().size());
	ZeroMemory(_current_key_index_map.data(), _current_key_index_map.size() * sizeof(uint));

	if (_animation == nullptr)
	{
		LOG_ERROR("Failed load Animation");
		return;
	}	
}

void Animator::Animate()
{
	bool isMMd = _animation->IsMMD();
	auto& transforms = _root->Get_Transform_Array();

	for (int i = 0; i < _animation->Get_Channels().size(); i++)
	{
		auto transform = transforms[i];
		if (transform)
		{
			auto key = Get_KeyFrame_Matrix(i);
			if (key.frame == -1)
				continue;
			if (isMMd)
			{
				transform->SetAnimPosition(key.pos);
				transform->SetAnimRotation(key.rot);
			}
			else {
				transform->SetAnimPosition_FBX(key.pos);
				transform->SetAnimRotation(key.rot);
			}
		}
	}
}

Bone_Key Animator::Get_KeyFrame_Matrix(int index)
{
	if (_animation == nullptr)
	{
		LOG_WARNING("No Animator");
		return Bone_Key();
	}

	auto& channel = _animation->Get_Channel(index);

	if (channel.keys.size() <= 0)
		return Bone_Key();
	else if (channel.keys.size() == 1)
		return channel.Get_Key(0);

	auto result = Update_Key_Index(index, channel);
	
	return Interporate_Key(channel.Get_Key(result.first), channel.Get_Key(result.second));
}

std::pair<uint, uint> Animator::Update_Key_Index(uint frame_index, const Bone_Channel& channel)
{
	uint maximum_index = channel.keys.size();
	uint& current_index = _current_key_index_map[frame_index];
	uint next_index = 1;
	// mixamo 는 처음과 끝이 같은 프레임이라 1로 해야함
	if (current_index + 1 >= maximum_index)
	{
		// 아직 frame 은 처음으로 안돌아옴
		if (_current_frame > channel.Get_Key(current_index).frame)		
			return { current_index, next_index };
	}
	else {
		next_index = current_index + 1;
	}

	while (channel.Get_Key(next_index).frame < _current_frame)
	{
		current_index = (current_index + 1) >= maximum_index ? 1 : current_index + 1;
		next_index = (current_index + 1) >= maximum_index ? 1 : current_index + 1;
		// 아직 frame 이 처음으로 안들어가면
		if (current_index > next_index && _current_frame > channel.Get_Key(current_index).frame)	
			break;	
	}

	return { current_index, next_index };
}

Bone_Key Animator::Interporate_Key(const Bone_Key& cur,const Bone_Key& after)
{
	Bone_Key result;
	float delta = _current_accumulated_delta_time_ms / _animation->Get_MsPerTic();
	delta += (_current_frame - cur.frame) > 0 ? (_current_frame - cur.frame - 1) : (_animation->Get_Duration() - cur.frame + _current_frame - 1);	
	delta /= (after.frame - cur.frame) > 0 ? (after.frame - cur.frame) : (_animation->Get_Duration() - cur.frame + after.frame);
	delta = delta > 1 ? 1 : delta;

	if (cur.bezier_x[0].x != -1)
	{
		result.pos.x = Math::Lerp(cur.pos.x, after.pos.x, Math::Bezier3(Vector2(0), cur.bezier_x[0], cur.bezier_x[1], Vector2(1, 1), delta).y);
		result.pos.y = Math::Lerp(cur.pos.y, after.pos.y, Math::Bezier3(Vector2(0), cur.bezier_y[0], cur.bezier_y[1], Vector2(1, 1), delta).y);
		result.pos.z = Math::Lerp(cur.pos.z, after.pos.z, Math::Bezier3(Vector2(0), cur.bezier_z[0], cur.bezier_z[1], Vector2(1, 1), delta).y);
		// 조금이라도 빨리할려고 slerp 말고 lerp 씀. 별로 차이도 안나는데 ㅇㅇ
		result.rot = Math::Lerp(cur.rot, after.rot, Math::Bezier3(Vector2(0), cur.bezier_rot[0], cur.bezier_rot[1], Vector2(1, 1), delta).y);
		result.frame = cur.frame + delta;
	}
	else
	{
		result.pos = Math::Lerp(cur.pos, after.pos, delta);
		result.scale = Math::Lerp(cur.scale, after.scale, delta);
		result.rot = Math::Lerp(cur.rot, after.rot, delta);
		result.frame = cur.frame + delta;
	}

	return result;
}
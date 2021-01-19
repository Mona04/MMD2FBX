#include "Framework.h"
#include "Animator.h"

#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/IKSolver.h"
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
			if (_current_frame > _animation->Get_Duration())
			{
				if (_animation->IsLoop())
				{
					_current_frame = 1; 
					ZeroMemory(_current_key_index_map.data(), _current_key_index_map.size() * sizeof(uint));
				}
				else
				{
					_isEnd = true;
					_current_frame = _animation->Get_Duration();
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

	_isEnd = false;
	_isPlaying = false;

	_animation = nullptr;
}

void Animator::SetAnimation(std::wstring_view path)
{
	Clear();

	auto mgr = _context->GetSubsystem<ResourceManager>();

	_animation = mgr->GetResource<Animation>(path);
	if (_animation == nullptr)
	{
		LOG_ERROR("Failed load Animation");
		_actor->GetSetting()->Set_UseIK(false);
		return;
	}

	_root = _actor->GetComponent<Transform>();

	_current_key_index_map.resize(_animation->Get_Channels().size());
	ZeroMemory(_current_key_index_map.data(), _current_key_index_map.size() * sizeof(uint));

	// setting Actor
	_actor->GetSetting()->Set_UseIK(_animation->UseIK());
	_actor->GetSetting()->Set_UsePhysics(_animation->UsePhysics());

	// init animation
	Animate();
	_root->Update();
	_actor->GetComponent<IKSolver>()->Update();
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

std::pair<uint, uint> Animator::Update_Key_Index(uint channel_index, const Bone_Channel& channel)
{
	if (_isEnd)
		return { _current_frame, _current_frame };

	uint maximum_index = channel.keys.size() - 1;
	uint& current_index = _current_key_index_map[channel_index];  // currnet_index 는 저장해야함
	uint next_index = 0;  // 

	if (current_index + 1 >= maximum_index) // 다음 cur_index 가 마지막의 경우
	{
		if (_animation->IsLoop())
		{
			// 그리고 cur_index 를 올려야할 경우. return ( cur, 0)
			if (_current_frame > channel.Get_Key(current_index).frame)
			{
				current_index++;
				return { current_index, next_index };
			}
		}
		else {
			return { current_index + 1, current_index + 1 };
		}
	}
	else { // Normal Case or cur_index < current_frame < next_index
		next_index = current_index + 1;
	}

	while (channel.Get_Key(next_index).frame < _current_frame)
	{		
		//  next_index < current_index < current_framel 의 경우 제외
		if (current_index > next_index && _current_frame > channel.Get_Key(current_index).frame)
			break;
		current_index = (current_index + 1) > maximum_index ? 1 : current_index + 1;
		next_index = (current_index + 1) > maximum_index ? 1 : current_index + 1;
	}

	return { current_index, next_index };
}

Bone_Key Animator::Interporate_Key(const Bone_Key& cur, const Bone_Key& after)
{
	Bone_Key result;
	float delta = _current_accumulated_delta_time_ms / _animation->Get_MsPerTic();
	delta += (_current_frame - cur.frame) > 0 ? (_current_frame - cur.frame) : (_animation->Get_Duration() - cur.frame + _current_frame);	
	delta /= (after.frame - cur.frame) > 0 ? (after.frame - cur.frame) : (_animation->Get_Duration() - cur.frame + after.frame);
	delta = delta > 1 ? 1 : delta;

	if (cur.bezier_x[0].x != -1)
	{
		result.pos.x = Math::Lerp(cur.pos.x, after.pos.x, Math::Bezier3(Vector2(0), cur.bezier_x[0], cur.bezier_x[1], Vector2(1, 1), delta).y);
		result.pos.y = Math::Lerp(cur.pos.y, after.pos.y, Math::Bezier3(Vector2(0), cur.bezier_y[0], cur.bezier_y[1], Vector2(1, 1), delta).y);
		result.pos.z = Math::Lerp(cur.pos.z, after.pos.z, Math::Bezier3(Vector2(0), cur.bezier_z[0], cur.bezier_z[1], Vector2(1, 1), delta).y);
		result.rot = Math::Lerp(cur.rot, after.rot, Math::Bezier3(Vector2(0), cur.bezier_rot[0], cur.bezier_rot[1], Vector2(1, 1), delta).y);
		result.frame = cur.frame + delta;
	}
	else
	{
		result.pos = Math::Lerp(cur.pos, after.pos, delta);
		result.rot = Math::Slerp(cur.rot, after.rot, delta);
		result.frame = cur.frame + delta;
	}

	return result;
}
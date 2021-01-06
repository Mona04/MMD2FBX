#include "Framework.h"
#include "MMD2FBX_Converter.h"

#include "SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/Actor.h"
#include "Scene/Component/Animator.h"

#include "Resource/Importer/MMD_Importer.h"
#include "Resource/Animation.h"

using namespace Framework;


MMD2FBX_Converter::MMD2FBX_Converter(Context* context) 
	: ISubsystem(context), _end_frame(-1), _start_frame(0), _cur_frame(0)
{
	EventSystem::GetInstance().Subscribe(EventSystem::EventType::Init, EVENT_HANDLER(Init));
	EventSystem::GetInstance().Subscribe(EventSystem::EventType::Update, EVENT_HANDLER(Update));
}

bool MMD2FBX_Converter::Init()
{	
	return true;
}

void MMD2FBX_Converter::Clear(bool not_frame)
{
	if (not_frame == false)
	{
		_start_frame = 0;
		_cur_frame = 0;
		_end_frame = -1;
	}

	_is_complete = false;
	_actor = nullptr;

	_exporter.Clear();
	_binary_exporter.Clear();
	
	_context->GetSubsystem<ResourceManager>()->Clear();
	auto sceneMgr = _context->GetSubsystem<SceneManager>();
	if(sceneMgr->GetCurrentScene())
		sceneMgr->EraseScene(sceneMgr->GetCurrentScene()->GetName());
}

void MMD2FBX_Converter::Update()
{
	if (_actor == nullptr)
	{
		_is_complete = true;
		return;
	}

	auto animator = _actor->GetComponent<Animator>();
	if (animator == nullptr)
	{
		LOG_ERROR("No animator");
		_is_complete = true;
		return;
	}

	if (animator->IsEnd())
	{
		Export();
		_is_complete = true;
		return;
	}

	Record();
	_cur_frame += 1;


	if(_end_frame > 0 && _cur_frame > _end_frame)
	{
		Export();
		_is_complete = true;
		return;
	}
}

void MMD2FBX_Converter::Record()
{
	if(_is_for_binary)
		_binary_exporter.Record_Animation_Frame(_cur_frame - _start_frame);
	else
		_exporter.Record_Animation_Frame(_cur_frame - _start_frame);
}

void MMD2FBX_Converter::Export()
{
	if (_is_for_binary)
	{
		auto file_path = FileSystem::GetIntactFileNameFromPath(_current_anim_path) + Extension_AnimationW;
		_binary_exporter.Export(file_path, _context);
	}
	else
	{
		auto file_path = FileSystem::GetIntactFileNameFromPath(_current_anim_path) + L".fbx";
		_exporter.Export(FileSystem::ToString(file_path));
	}

	Clear(false);
}

void MMD2FBX_Converter::SetSampleActor(const std::wstring& model_path, const std::wstring& anim_path)
{
	Clear(true);
	
	_context->GetSubsystem<SceneManager>()->AddSampleScene();
	auto scene = _context->GetSubsystem<SceneManager>()->GetCurrentScene();

	if (scene)
	{
		_actor = scene->FindActor("Sample");
		_current_anim_path = anim_path;

		bool r = false;
		{
			MMD_Importer importer;
			r = importer.Load_Model(model_path, _actor, _context);
		}
		if (r == false)
		{
			Clear(false); 
			return;
		}
		{
			MMD_Importer importer;
			r = importer.Load_Animation(anim_path, _actor, _context);
		}
		if (r == false)
		{
			Clear(false);
			return;
		}

		if (_is_for_binary)
		{
			_binary_exporter.Init(_actor, _start_frame, _end_frame);
			// fixed to original vmd animation pixel per tick;
			if (_end_frame < 0) // without end_frame setting
			{
				auto animation = _actor->GetComponent<Animator>()->GetAnimation();
				_end_frame = animation->Get_Duration() / animation->Get_MsPerTic() * _ms_per_tick;
			}
		}
		else
		{
			_exporter.Init(_actor);
			Set_Ms_per_Tick(_ms_per_tick);
			
			if (_end_frame < 0) // without end_frame setting
			{
				auto animation = _actor->GetComponent<Animator>()->GetAnimation();
				_end_frame = animation->Get_Duration();
			}
		}

		_actor->GetComponent<Animator>()->SetCurrentFrame(_start_frame);
	}	
}

void MMD2FBX_Converter::Set_StartFrame(int var)
{
	_start_frame = var;
	_cur_frame = var;
}

void MMD2FBX_Converter::Set_Ms_per_Tick(float var)
{
	_ms_per_tick = var;
	_exporter.Set_ms_per_tick(var);
	if (_actor)
	{
		auto animator = _actor->GetComponent<Animator>();
		animator->SetMsPerTick(_ms_per_tick);
	}
}

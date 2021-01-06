#include "Framework.h"
#include "Engine.h"


#include "Util/EventSystem.h"

#include "Subsystem/Function/Thread.h"
#include "Subsystem/Function/Timer.h"
#include "Subsystem/Resource/ResourceManager.h"
#include "Subsystem/Scene/SceneManager.h"
#include "Subsystem/Scene/MMD2FBX_Converter.h"

#include <functional>

using namespace Framework;

Engine::Engine() : _timer(nullptr), _converter(nullptr)
{
}

Engine::~Engine()
{
}

void Engine::Init(unsigned int screenWidth, unsigned int screenHeight, bool vsync, HWND hwnd, bool fullScreen, float screenDepth, float screenNear)
{
	_context = std::make_unique<Context>();
	_timer   = _context->AddSubsystem<Timer>();

	_context->AddSubsystem<Thread>();
	_context->AddSubsystem<ResourceManager>();// as least initialized already before sceneManager
	_context->AddSubsystem<SceneManager>();  // last one

	_converter = _context->AddSubsystem<MMD2FBX_Converter>(); // scene update 후에 해야지 동작 적용됨

	EventSystem::GetInstance().Fire(EventSystem::EventType::Init);
	_context->GetSubsystem<SceneManager>()->AddSampleScene();
}

void Engine::Update()
{
	if (_is_run == false)
		return;

	_timer->Update();
	
	if (_context->GetSubsystem<MMD2FBX_Converter>()->IsComplete())
	{
		if (_anim_paths.size() > 0)
		{
			auto anim_path = _anim_paths.front();
			_anim_paths.pop();
			_converter->SetSampleActor(_model_path, anim_path);
		}
		else {
			_is_run = false;
			return;
		}
	}

	if (_timer->IsUpdatedFPS())
	{
		EventSystem::GetInstance().Fire(EventSystem::EventType::Update);
		EventSystem::GetInstance().Fire(EventSystem::EventType::Render);
	}
}

void Engine::Set_AnimPath(const std::wstring& path)
{
	while (_anim_paths.empty() == false)
		_anim_paths.pop();
	_anim_paths.push(path);
}

void Engine::Set_AnimFolder(const std::wstring& path)
{
	while (_anim_paths.empty() == false)
		_anim_paths.pop();

	auto files = FileSystem::GetFilesInDirectory(path);
	for (const auto& file : files)
		_anim_paths.push(file);
}

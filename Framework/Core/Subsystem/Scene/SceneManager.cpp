#include "Framework.h"
#include "SceneManager.h"

#include "Scene/Scene.h"
#include "Scene/Actor.h"


using namespace Framework;

void SceneManager::AddSampleScene()
{
	auto cur_scene = AddScene("Sample Scene");

	// 이거 안해서 이십분 버렸다.
	SetCurrentScene(cur_scene->GetName());

	auto actor = cur_scene->AddSampleActor();
}

SceneManager::SceneManager(Context* context) : ISubsystem(context)
{
	EventSystem::GetInstance().Subscribe(EventSystem::EventType::Init, EVENT_HANDLER(Init));
	EventSystem::GetInstance().Subscribe(EventSystem::EventType::Update, EVENT_HANDLER(Update));
}

bool SceneManager::Init()
{
	// 생성자에 넣으면 충돌남
	_default_camera = std::make_shared<Actor>(_context);

	return true;
}

void SceneManager::Update()
{
	_default_camera->Update();
	for (auto& scene : _scenes)
		scene.second->Update();
}

std::shared_ptr<class Scene> SceneManager::AddScene(const std::string& name)
{
	//Set Current Scene 은 별도로 해줘야하니까 주의
	if (_scenes.find(name) == std::end(_scenes))
	{
		_scenes[name] = std::make_shared<Scene>(_context);
		_scenes[name]->SetName(name);
	}
	return _scenes[name];
}

std::shared_ptr<class Scene> SceneManager::GetScene(const std::string& name)
{
	if (_scenes.find(name) != std::end(_scenes))
		return _scenes[name];
	else
		return std::shared_ptr<class Scene>(nullptr);
}

void SceneManager::EraseScene(const std::string& name)
{
	if (_scenes.find(name) != std::end(_scenes))
	{
		auto erased = *_scenes.find(name);
		_scenes.erase(_scenes.find(name));
		if (_scene_current->GetCode() == erased.second->GetCode())
			_scene_current = nullptr;
	}
}

void SceneManager::Clear_Scenes()
{
	_scene_current.reset();
	_scenes.clear();
}

std::shared_ptr<class Scene> SceneManager::GetCurrentScene()
{
	return _scene_current;
}

bool SceneManager::SetCurrentScene(const std::string& name)
{
	if (_scenes.find(name) != std::end(_scenes))
	{
		_scene_current = _scenes[name];
		return true;
	}
	return false;
}
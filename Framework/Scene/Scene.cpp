#include "Framework.h"
#include "Scene.h"
#include "Actor.h"
#include <algorithm>


using namespace Framework;

Scene::~Scene()
{
	Clear();
}

void Scene::Clear()
{
	for (auto& actor : _actors)
		SAFE_DELETE(actor);
}

bool Scene::Update()
{
	for (auto& actor : _actors)
		actor->Update();

	return true;
}

class Actor* Scene::AddSampleActor()
{
	auto actor = _actors.emplace_back(new Actor(_context));
	actor->SetName("Sample");
	actor->Init_Sample();
	return actor;
}

Actor* Scene::AddEditorCamera()
{
	auto actor = _actors.emplace_back(new Actor(_context));
	return nullptr;
}

class Actor* Scene::AddActor()
{
	return _actors.emplace_back(new Actor(_context));
}

class Actor* Scene::FindActor(std::string_view name)
{
	auto finded = std::find_if(_actors.begin(), _actors.end(),
		[name](Actor* in) { if (in->GetName() == name) return true; else return false; });
	
	if (finded != _actors.end())
		return (*finded);
	else
		return nullptr;
}

class Actor* Scene::FindActor(unsigned int code)
{
	auto finded = std::find_if(_actors.begin(), _actors.end(),
		[code](Actor* in) { if (in->GetCode() == code) return true; else return false; });
	if (finded != _actors.end())
		return (*finded);
	else
		return nullptr;
}

void Scene::DeleteActor(unsigned int code)
{
	auto finded = std::find_if(_actors.begin(), _actors.end(),
		[code](Actor* in) { if (in->GetCode() == code) return true; else return false; });

	if (finded != _actors.end())
	{
		Actor* tmp = *finded;
		_actors.erase(finded);
		SAFE_DELETE(tmp);
	}
}
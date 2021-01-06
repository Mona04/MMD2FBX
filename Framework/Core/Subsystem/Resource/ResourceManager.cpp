#include "Framework.h"
#include "ResourceManager.h"

using namespace Framework;

ResourceManager::ResourceManager(Context* context) : ISubsystem(context)
{
	EventSystem::GetInstance().Subscribe(EventSystem::EventType::Init, EVENT_HANDLER(Init));
	EventSystem::GetInstance().Subscribe(EventSystem::EventType::Update, EVENT_HANDLER(Update));
}

bool ResourceManager::Init()
{
	_map_resources.insert(std::make_pair(IResource::TypeCode::Script, Resources()));
	_map_resources.insert(std::make_pair(IResource::TypeCode::Texture, Resources()));
	_map_resources.insert(std::make_pair(IResource::TypeCode::Mesh, Resources()));
	_map_resources.insert(std::make_pair(IResource::TypeCode::SkeletalMesh, Resources()));
	_map_resources.insert(std::make_pair(IResource::TypeCode::Skeleton, Resources()));
	_map_resources.insert(std::make_pair(IResource::TypeCode::Material, Resources()));
	_map_resources.insert(std::make_pair(IResource::TypeCode::Animation, Resources()));
	return true;
}

void ResourceManager::Update()
{

}

void ResourceManager::Clear()
{
	for (auto& map : _map_resources)
	{
		map.second.clear();
	}
}

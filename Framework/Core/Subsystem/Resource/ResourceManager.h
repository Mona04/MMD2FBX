#pragma once
#include <unordered_map>
#include <type_traits>
#include <assert.h>
#include "Core/Subsystem/ISubsystem.h"
#include "Resource/IResource.h"
#include "Util/FileSystem.h"

namespace Framework
{
	class ResourceManager : public ISubsystem
	{
		typedef std::unordered_map<std::wstring, std::shared_ptr<IResource>> Resources;
		typedef std::unordered_map<IResource::TypeCode, Resources> Map_Resources;
	public:
		ResourceManager(class Context* context);
		virtual ~ResourceManager() = default;

		virtual bool Init() override;
		virtual void Update() override;
		void Clear();

		template<typename T> Resources& GetResources();
		template<typename T> std::shared_ptr<T> GetResource(std::wstring_view path);
		template<typename T> std::shared_ptr<T> RemoveResource(std::wstring_view path);
		template<typename T> std::shared_ptr<T> FindResource(std::wstring_view path);
		// Resource made extern of Manager be Registered
		template<typename T> bool RegisterResource(std::shared_ptr<T> src, std::wstring_view path);

	private:
		template<typename T> std::shared_ptr<T> LoadFromFile(std::wstring_view path);
		template<typename T> bool SaveToFile(std::wstring_view path);

	private:
		Map_Resources _map_resources;
	};


	template<typename T>
	inline std::unordered_map<std::wstring, std::shared_ptr<IResource>>& ResourceManager::GetResources()
	{
		static_assert(std::is_base_of_v<IResource, T>);

		IResource::TypeCode code = IResource::DeduceType<T>();
		return _map_resources.find(code)->second;
	}

	template<typename T>
	inline std::shared_ptr<T> ResourceManager::GetResource(std::wstring_view path)
	{
		static_assert(std::is_base_of_v<IResource, T>);

		if (FileSystem::IsExistFile(path) == false)
			return nullptr;
		
		Resources resources = GetResources<T>(); 
		auto iter = resources.find(std::wstring(path));

		if (iter != resources.end() && iter->second != nullptr && 
			IResource::DeduceType<T>() != IResource::TypeCode::Script)
		{
			LOG_INFO("Already Created Source - " + FileSystem::ToString(path));
			return std::static_pointer_cast<T>(iter->second);
		}
		else
		{
			LOG_INFO("Create Source - " + FileSystem::ToString(path));
			return LoadFromFile<T>(path);
		}
	}

	template<typename T>
	inline std::shared_ptr<T> ResourceManager::RemoveResource(std::wstring_view path)
	{
		static_assert(std::is_base_of<IResource, T>::value, "Provided type does not implement IResource");
		
		Resources& resources = GetResources<T>();

		auto iter = std::find_if(resources.begin(), resources.end(),
			[path](auto var) { return var.second->GetPath() == path; });

		if (iter != resources.end())
		{
			auto result = iter->second;
			resources.erase(iter);
			return std::static_pointer_cast<T>(result);
		}

		return nullptr;;
	}

	template<typename T>
	inline std::shared_ptr<T> ResourceManager::FindResource(std::wstring_view path)
	{
		static_assert(std::is_base_of_v<IResource, T>);

		if (FileSystem::IsExistFile(Relative_BasisW + std::wstring(path)) == false)
			return nullptr;

		Resources resources = GetResources<T>();
		auto iter = resources.find(std::wstring(path));

		if (iter != resources.end() && iter->second != nullptr &&
			IResource::DeduceType<T>() != IResource::TypeCode::Script)
		{
			LOG_INFO("Already Created Source - " + FileSystem::ToString(path));
			return std::static_pointer_cast<T>(iter->second);
		}
		else
			return nullptr;
	}

	template<typename T>
	inline bool ResourceManager::RegisterResource(std::shared_ptr<T> src, std::wstring_view path)
	{
		static_assert(std::is_base_of_v<IResource, T>);
		if (src == nullptr) return false;

		if (this->FindResource<T>(path) != nullptr)
			RemoveResource<T>(path);

		src->SetPath(path);

		src->SaveToFile(std::wstring(path));
		src->LoadFromFile(std::wstring(path)); // ¿©±â¼­ buffer ¸¸µë. ±×·¯´Ï±î »©¸é ¾ÈµÊ.

		return true;
	}

	template<typename T>
	inline std::shared_ptr<T> ResourceManager::LoadFromFile(std::wstring_view path)
	{
		static_assert(std::is_base_of_v<IResource, T>);

		if (FileSystem::IsExistFile(path) == false)
		{
			LOG_WARNING("Cannot find file. path : " + FileSystem::ToString(path));
			return nullptr;
		}
		std::shared_ptr<T> new_one = std::make_shared<T>(_context);

		new_one->SetPath(path);
		new_one->LoadFromFile(path);

		Resources& resources = GetResources<T>();
		resources.insert(std::make_pair(path, new_one));

		return new_one;
	}

	template<typename T>
	inline bool ResourceManager::SaveToFile(std::wstring_view path)
	{
		static_assert(std::is_base_of_v<IResource, T>);

		return std::shared_ptr<IResource>();
	}
}
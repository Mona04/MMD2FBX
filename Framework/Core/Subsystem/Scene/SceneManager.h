#pragma once
#include <unordered_map>
#include "Core/Subsystem/ISubsystem.h"

namespace Framework
{
	class SceneManager : public ISubsystem
	{
	public:
		void AddSampleScene();
		
	public:
		SceneManager(class Context* context);
		virtual ~SceneManager() {}

		virtual bool Init() override;
		virtual void Update() override;

		std::shared_ptr<class Scene> AddScene(const std::string& name);
		std::shared_ptr<class Scene> GetScene(const std::string& name);
		void EraseScene(const std::string& name);

		void Clear_Scenes();
		std::shared_ptr<class Scene> GetCurrentScene();
		bool SetCurrentScene(const std::string& name);


	protected:
		std::unordered_map<std::string, std::shared_ptr<class Scene>> _scenes;
		std::shared_ptr<class Scene> _scene_current;
		std::shared_ptr<class Actor> _default_camera;
	};
}
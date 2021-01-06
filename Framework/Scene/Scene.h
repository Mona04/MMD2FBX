#pragma once
#include <vector>
#include <memory>
#include "Chimera_Object.h"

namespace Framework
{
	class Scene : public Chimera_Object
	{
	public:
		Scene(class Context* context) : _context(context), Chimera_Object() {};
		virtual ~Scene();
		// Scene interact with Editor so much that we should make constitute raw pointer

		Scene(Scene& rhs) = delete;
		Scene(Scene&& rhs) = delete;
		Scene& operator=(Scene& rhs) = delete;
		Scene& operator=(Scene&& rhs) = delete;

		void Clear();
		bool Update();

		class Actor* AddSampleActor();
		class Actor* AddEditorCamera();
		class Actor* AddActor();
		class Actor* FindActor(std::string_view name);
		class Actor* FindActor(unsigned int code);
		void DeleteActor(unsigned int code);

		class Camera* GetCamera();

		std::vector<class Actor*>& GetActors() { return _actors; }

	protected:
		class Context* _context;
		std::vector<class Actor*> _actors;
	};
}
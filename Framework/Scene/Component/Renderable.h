#pragma once
#include "IComponent.h"

namespace Framework
{
	class Renderable : public IComponent
	{
	public:
		Renderable(class Context* context);
		virtual ~Renderable();

		void LoadFromFile(std::wstring_view path);
		void SaveToFile(std::wstring_view path);

		virtual bool Update();
		virtual void Clear();

		std::vector<std::shared_ptr<class Mesh>>& GetMeshes() { return _meshes; }
		void SetMeshes(std::vector<std::wstring_view>& paths);
		void AddMesh();
		void AddMesh(std::wstring_view mesh);
		void DeleteMesh(uint i);

		std::vector<std::shared_ptr<class Material>>& GetMaterials() { return _materials; }
		void SetMaterials(std::vector<std::wstring_view>& paths);
		void AddMaterial();
		void AddMaterial(std::wstring_view path);
		void DeleteMaterial(uint i );;

		void SetRenderMesh();
		void SetCube();
		void SetGizmo();
		void SetGrid();

		bool IsMMD() { return _isMMD; }
		void SetIsMMD(bool var) { _isMMD = var; }
		bool IsFBX() { return _isFBX; }
		void SetIsFBX(bool var) { _isFBX = var; }

	protected:
		std::vector<std::shared_ptr<class Mesh>> _meshes;    
		std::vector<std::shared_ptr<class Material>> _materials;

		bool _isMMD = false;
		bool _isFBX = false;
	};
}
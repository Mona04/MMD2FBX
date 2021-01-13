#pragma once
#include "IComponent.h"
#include <map>

namespace Framework
{
	class Renderable : public IComponent
	{
	protected:
		struct Morph_Package
		{
			Morph_Package() : weight(-1), morph(nullptr) {}
			Morph_Package(float weight, std::shared_ptr<class Morph> morph)
				: weight(weight), morph(morph) {}
			float weight;
			std::shared_ptr<class Morph> morph;
		};

	public:
		Renderable(class Context* context);
		virtual ~Renderable();

		void LoadFromFile(std::wstring_view path);
		void SaveToFile(std::wstring_view path);

		virtual bool Update();
		virtual void Clear();

	public:
		std::vector<std::shared_ptr<class Mesh>>& GetMeshes() { return _meshes; }
		void SetMeshes(std::vector<std::wstring_view>& paths);
		void AddMesh();
		void AddMesh(std::wstring_view mesh);
		void DeleteMesh(uint i);

		std::vector<std::shared_ptr<class Material>>& GetMaterials() { return _materials; }
		void SetMaterials(std::vector<std::wstring_view>& paths);
		void AddMaterial();
		void AddMaterial(std::wstring_view path);
		void DeleteMaterial(uint i);;

		std::map<std::wstring, Morph_Package>& GetMorphs() { return _morphs; }
		void SetMorphs(std::vector<std::wstring_view>& paths);
		void AddMorph(std::wstring_view path);
		void DeleteMorph(uint i);;

	public:
		bool IsMMD() { return _isMMD; }
		void SetIsMMD(bool var) { _isMMD = var; }
		bool IsFBX() { return _isFBX; }
		void SetIsFBX(bool var) { _isFBX = var; }
		bool IsMorphed() { return _isMorphed; }

	public:
		void SetRenderMesh();
		void SetCube();
		void SetGizmo();
		void SetGrid();

	protected:
		std::vector<std::shared_ptr<class Mesh>> _meshes;
		std::vector<std::shared_ptr<class Material>> _materials;
		std::map<std::wstring, Morph_Package> _morphs;

		bool _isMMD = false;
		bool _isFBX = false;
		bool _isMorphed = false;
	};
}
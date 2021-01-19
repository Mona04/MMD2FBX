#pragma once
#include <vector>
#include <cstddef>
#include <memory>
#include "IResource.h"

namespace Framework
{
	class Mesh : public IResource, public std::enable_shared_from_this<Mesh>
	{
	public:
		Mesh(Context* context);
		virtual ~Mesh();

		Mesh(Mesh& rhs) = delete;
		Mesh(Mesh&& rhs) = delete;
		Mesh& operator=(Mesh& rhs) = delete;
		Mesh& operator=(Mesh&& rhs) = delete;

		virtual bool LoadFromFile(std::wstring_view path) override;
		virtual bool SaveToFile(std::wstring_view path) const override;
		virtual void Clear() override;

		virtual std::vector<struct Vertex_MASTER>& GetVertices() { return _vertices; }
		virtual std::vector<uint>& GetIndices() { return _indices; }

		virtual void Set_Material_Index(int var) { _material_index = var; }
		virtual int Get_Material_Index() { return _material_index; }

	private:
		virtual bool CreateBuffer();

	protected:
		std::vector<struct Vertex_MASTER> _vertices;
		std::vector<uint> _indices;
		int _material_index = -1;    // fbx ¿ë
	};
}
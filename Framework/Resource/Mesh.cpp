#include "Framework.h"
#include "Mesh.h"
#include "Util/Geomerty_Generator.h"
#include "Core/DirectX/0_IADesc/Input_Desc.h"

using namespace Framework;

Mesh::Mesh(Context* context) : IResource(context)
{
	_typecode = TypeCode::Mesh;
}

Mesh::~Mesh()
{
	Clear();
}

bool Mesh::LoadFromFile(std::wstring_view path)
{
	Clear();

	FileStream stream;
	stream.Open(std::wstring(path), StreamMode::Read);
	{
		stream.Read(_material_index);
		{
			int size = stream.ReadUInt();
			if (size > 10000000) return false;
			_vertices.reserve(size);
			_vertices.resize(size);
			for (auto& v : _vertices)
			{
				stream.Read(v.pos);
				stream.Read(v.uv);
				stream.Read(v.normal);
				stream.Read(v.color);
			}
		}
		
		{		
			int size = stream.ReadUInt();
			if (size > 10000000) return false;
			_indices.reserve(size);
			_indices.resize(size);
			for (auto& indice : _indices)
			{
				stream.Read(indice);
			}	
		}

	}
	stream.Close();

	CreateBuffer();
	return true;
}

bool Mesh::SaveToFile(std::wstring_view path)
{
	FileStream stream;
	stream.Open(std::wstring(path), StreamMode::Write);
	{
		stream.Write(_material_index);
		stream.Write(static_cast<uint>(_vertices.size()));
		for (auto& v : _vertices)
		{
			stream.Write(v.pos);
			stream.Write(v.uv);
			stream.Write(v.normal);
			stream.Write(v.color);
		}

		stream.Write(static_cast<uint>(_indices.size()));
		for (auto& i : _indices)
		{
			stream.Write(i);
		}
	}
	stream.Close();
	return true;
}


void Mesh::Clear()
{
	_vertices.clear();
	_indices.clear();
}

bool Mesh::CreateBuffer()
{
	return true;
}

void Mesh::MakeCube()
{
	//새로 Mesh 를 만들 필요가 있으면 이렇게 만들거나 따로 빼거나. 
	_vertices = Geometry_Generator::Cube_Vertices<Vertex_MASTER>(0.01f);
	_indices = Geometry_Generator::Cube_Indices();
	SaveToFile(Relative_BasisW + L"_Assets/Model/Cube" + Extension_MeshW);
	CreateBuffer();
}

void Mesh::MakeGizmo()
{
	_vertices = Geometry_Generator::Gizmo_Vertices<Vertex_MASTER>(0.005f, 0.5f);
	_indices = Geometry_Generator::Gizmo_Indices();
	SaveToFile(Relative_BasisW + L"_Assets/Model/Gizmo" + Extension_MeshW);
	CreateBuffer();
}

void Mesh::MakeScreenMesh()
{
	//새로 Mesh 를 만들 필요가 있으면 이렇게 만들거나 따로 빼거나. 
	_vertices = Geometry_Generator::ScreenQuad_Vertices<Vertex_MASTER>();
	_indices = Geometry_Generator::ScreenQuad_Indices();
	SaveToFile(Relative_BasisW + L"_Assets/Model/Screen" + Extension_MeshW);
	CreateBuffer();
}

void Mesh::MakeGrid()
{
	//새로 Mesh 를 만들 필요가 있으면 이렇게 만들거나 따로 빼거나. 
	_vertices = Geometry_Generator::Grid_Vertices<Vertex_MASTER>();
	_indices = Geometry_Generator::Grid_Indices();
	SaveToFile(Relative_BasisW + L"_Assets/Model/Grid" + Extension_MeshW);
	CreateBuffer();
}
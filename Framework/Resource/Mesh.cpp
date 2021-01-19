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

bool Mesh::SaveToFile(std::wstring_view path) const
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
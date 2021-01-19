#include "Framework.h"
#include "SkeletalMesh.h"

#include "Core/DirectX/0_IADesc/Input_Desc.h"

#include "Util/Geomerty_Generator.h"


using namespace Framework;

SkeletalMesh::SkeletalMesh(Context* context) : Mesh(context)
{
	_typecode = TypeCode::SkeletalMesh;
}

SkeletalMesh::~SkeletalMesh()
{
}

bool SkeletalMesh::LoadFromFile(std::wstring_view path)
{
	Clear();

	FileStream stream;
	stream.Open(std::wstring(path), StreamMode::Read);
	{
		stream.Read(_material_index);
		{
			int size = stream.ReadUInt(); if (size > 10000000) return false;
			_vertices.reserve(size);
			_vertices.resize(size);
			for (auto& v : _vertices)
			{
				stream.Read(v.pos);
				stream.Read(v.uv);
				stream.Read(v.color);
				stream.Read(v.uva[0]); stream.Read(v.uva[1]); stream.Read(v.uva[2]); stream.Read(v.uva[3]);
				stream.Read(v.normal); stream.Read(v.tangent), stream.Read(v.binormal);
				stream.Read(v.bone_index[0]); stream.Read(v.bone_index[1]); stream.Read(v.bone_index[2]); stream.Read(v.bone_index[3]);
				stream.Read(v.bone_weight[0]); stream.Read(v.bone_weight[1]); stream.Read(v.bone_weight[2]); stream.Read(v.bone_weight[3]);

				if (isnan(v.normal.x) || isinf(v.normal.x) || isnan(v.tangent.x) || isinf(v.tangent.x)
					|| isinf(v.binormal.x) || isinf(v.binormal.x) || isnan(v.normal.Length()) || isinf(v.normal.Length()))
				{
					v.tangent = v.normal;
					v.tangent.x += 0.01f;
				}
			}
		}
		{
			int size = stream.ReadUInt(); if (size > 10000000) return false;
			_indices.reserve(size);
			_indices.resize(size);
			for (auto& indice : _indices)
			{
				stream.Read(indice);
			}
		}
	}
	stream.Close();

	return true;
}

bool SkeletalMesh::SaveToFile(std::wstring_view path) const
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
			stream.Write(v.color);
			stream.Write(v.uva[0]); stream.Write(v.uva[1]); stream.Write(v.uva[2]); stream.Write(v.uva[3]);
			stream.Write(v.normal); stream.Write(v.tangent), stream.Write(v.binormal);
			stream.Write(v.bone_index[0]); stream.Write(v.bone_index[1]); stream.Write(v.bone_index[2]); stream.Write(v.bone_index[3]);
			stream.Write(v.bone_weight[0]); stream.Write(v.bone_weight[1]); stream.Write(v.bone_weight[2]); stream.Write(v.bone_weight[3]);
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
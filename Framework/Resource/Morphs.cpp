#include "Framework.h"
#include "Morphs.h"

using namespace Framework;

Morphs::Morphs(Context* context) : IResource(context)
{
	_typecode = TypeCode::Morphs;

	Clear();
}

Morphs::~Morphs()
{
	Clear();
}

bool Morphs::LoadFromFile(std::wstring_view path)
{
	Clear();

	FileStream stream;
	stream.Open(std::wstring(path), StreamMode::Read);
	{
		uint count = stream.ReadUInt();
		for (int i = 0; i < count; i++)
			AddMorph()->LoadFromFile(stream);
	}
	stream.Close();
	return true;
}

bool Morphs::SaveToFile(std::wstring_view path) const
{
	FileStream stream;
	stream.Open(std::wstring(path), StreamMode::Write);
	{
		stream.Write(static_cast<uint>(_morphs.size()));
		for (const auto& m : _morphs)
			m->SaveToFile(stream);
	}
	stream.Close();

	return true;
}

void Morphs::Clear()
{
	_morphs.clear();
	_morphs.shrink_to_fit();
}

// ============================ Morph ===========================================
bool Morph::LoadFromFile(FileStream& stream)
{
	Clear();
	
	
	stream.Read(_morph_name);
	_morph_type = static_cast<MorphType>(stream.ReadUInt());
	_morph_category = static_cast<MorphCategory>(stream.ReadUInt());

	uint count = stream.ReadUInt();
	_vertex_offsets.resize(count);
	for (auto& v : _vertex_offsets)
		v.LoadFromFile(stream);

	count = stream.ReadUInt();
	_uv_offsets.resize(count);
	for (auto& v : _uv_offsets)
		v.LoadFromFile(stream);

	count = stream.ReadUInt();
	_group_offsets.resize(count);
	for (auto& v : _group_offsets)
		v.LoadFromFile(stream);

	count = stream.ReadUInt();
	_material_offsets.resize(count);
	for (auto& v : _material_offsets)
		v.LoadFromFile(stream);

	return true;
}

bool Morph::SaveToFile(FileStream& stream) const
{

	stream.Write(_morph_name);
	stream.Write(static_cast<uint>(_morph_type));
	stream.Write(static_cast<uint>(_morph_category));

	stream.Write(static_cast<uint>(_vertex_offsets.size()));
	for (const auto& v : _vertex_offsets)
		v.SaveToFile(stream);

	stream.Write(static_cast<uint>(_uv_offsets.size()));
	for (const auto& v : _uv_offsets)
		v.SaveToFile(stream);

	stream.Write(static_cast<uint>(_group_offsets.size()));
	for (const auto& v : _group_offsets)
		v.SaveToFile(stream);

	stream.Write(static_cast<uint>(_material_offsets.size()));
	for (const auto& v : _material_offsets)
		v.SaveToFile(stream);
	
	return true;
}

void Morph::Clear()
{
	_vertex_offsets.clear();
	_vertex_offsets.shrink_to_fit();
	_uv_offsets.clear();
	_uv_offsets.shrink_to_fit();
	_group_offsets.clear();
	_group_offsets.shrink_to_fit();
	_material_offsets.clear();
	_material_offsets.shrink_to_fit();

	_morph_name = L"";
	_morph_type = MorphType::Vertex;
	_morph_category = MorphCategory::Other;
}


// ============================ Offset SaveLoad ==================================
#pragma region

void Morph::Vertex_Offset::SaveToFile(FileStream& stream) const
{
	stream.Write(this->vertex_index);
	stream.Write(this->offset);
}

void Morph::Vertex_Offset::LoadFromFile(FileStream& stream)
{
	stream.Read(this->vertex_index);
	stream.Read(this->offset);
}

void Morph::UV_Offset::SaveToFile(FileStream& stream) const
{
	stream.Write(this->vertex_index);
	stream.Write(this->offset);
}

void Morph::UV_Offset::LoadFromFile(FileStream& stream)
{
	stream.Read(this->vertex_index);
	stream.Read(this->offset);
}

void Morph::Group_Offset::SaveToFile(FileStream& stream) const
{
	stream.Write(this->morph_index);
	stream.Write(this->weight);
}

void Morph::Group_Offset::LoadFromFile(FileStream& stream)
{
	stream.Read(this->morph_index);
	stream.Read(this->weight);
}

void Morph::Material_Offset::SaveToFile(FileStream& stream) const
{
	stream.Write(this->material_index);
	stream.Write(this->offset_operation);

	stream.Write(this->diffuse);
	stream.Write(this->specular);
	stream.Write(this->specularity);
	stream.Write(this->ambient);

	stream.Write(this->edge_color);
	stream.Write(this->edge_size);

	stream.Write(this->texture_rgba);
	stream.Write(this->sphere_texture_rgba);
	stream.Write(this->toon_texture_rgba);
}

void Morph::Material_Offset::LoadFromFile(FileStream& stream)
{
	stream.Read(this->material_index);
	stream.Read(this->offset_operation);
		   
	stream.Read(this->diffuse);
	stream.Read(this->specular);
	stream.Read(this->specularity);
	stream.Read(this->ambient);
		   
	stream.Read(this->edge_color);
	stream.Read(this->edge_size);
		   
	stream.Read(this->texture_rgba);
	stream.Read(this->sphere_texture_rgba);
	stream.Read(this->toon_texture_rgba);
}

#pragma endregion
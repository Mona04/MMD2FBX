#include "Framework.h"
#include "Material.h"

#include "Framework/Core/DirectX/0_IADesc/Input_Desc.h"

#include "Framework/Resource/Texture.h"
#include "Framework/Core/Subsystem/Resource/ResourceManager.h"

using namespace Framework;

Material::Material(Context* context)
	: IResource(context)
	, _index_count(0)
{
	_typecode = TypeCode::Material;
}

Material::~Material()
{
	Clear();
}

bool Material::LoadFromFile(std::wstring_view path)
{
	Clear();

	uint nTexture = 0;
	uint type_texture = 0;
	std::wstring texture_path;
	uint8_t flag;

	FileStream stream;
	stream.Open(std::wstring(path), StreamMode::Read);
	{
		stream.Read(_material_name);
		stream.Read(_material_english_name);

		stream.Read(nTexture);

		for (int i = 0 ; i < nTexture; i++)
		{
			stream.Read(type_texture);
			stream.Read(texture_path);
			Set_Texture(texture_path, static_cast<Type_Texture>(type_texture));
		}


		_material_common.LoadFromFile(stream);
		_material_mmd.LoadFromFile(stream);

		stream.Read(_index_count);
	}
	stream.Close();


	CreateBuffer();

	return true;
}

bool Material::SaveToFile(std::wstring_view path) const
{
	FileStream stream;
	stream.Open(std::wstring(path), StreamMode::Write);
	{
		stream.Write(_material_name);
		stream.Write(_material_english_name);
		
		uint nTexture = _textures.size();
		stream.Write(nTexture);

		for (auto iter = _textures.begin(); iter != _textures.end(); iter++)
		{
			auto type = iter->first;
			auto texture = iter->second;
			stream.Write(static_cast<uint>(type));
			if (texture)
				stream.Write(texture->GetPath());
			else
				stream.Write(None_StringW);
		}

		_material_common.SaveToFile(stream);
		_material_mmd.SaveToFile(stream);

		stream.Write(_index_count);
	}
	stream.Close();

	return true;
}

void Material::Clear()
{
}

void Material::Update_Buffers()
{
}

bool Material::CreateBuffer()
{
	return true;
}

void Material::Set_Texture(std::wstring_view path, Type_Texture type)
{
	auto texture = _context->GetSubsystem<ResourceManager>()->GetResource<Texture>(path);
	_textures[type] = texture;
}

std::wstring Material::Get_TexturePath(Type_Texture type)
{
	if (_textures.find(type) != _textures.end())
	{
		return _textures[type]->GetPath();
	}
	return None_StringW;
}

std::shared_ptr<class Texture> Material::Get_Texture(Type_Texture type)
{
	if (_textures.find(type) != _textures.end())
	{
		return _textures[type];
	}
	return nullptr;
}


//=================================================================================
// Material Datas
//=================================================================================

void Material_MMD::Clear()
{
	_draw_mode = PMXDrawMode::VertexColor;
	_sphere_op_mode = PMXSphereMode::Add; 
	_toon_mode = PMXToonMode::Common; 

	_texture_add_factor = { 0, 0, 0, 0 };
	_texture_mul_factor = { 1, 1, 1, 1 };
	_sphere_add_factor = { 0, 0, 0, 0 };
	_sphere_mul_factor = { 1, 1, 1, 1 };
	_toon_add_factor = { 0, 0, 0, 0};
	_toon_mul_factor = { 1, 1, 1, 1 };
}

void Material_MMD::SaveToFile(FileStream& stream) const
{
	stream.Write(static_cast<uint8_t>(_draw_mode));
	stream.Write(static_cast<uint8_t>(_sphere_op_mode));
	stream.Write(static_cast<uint8_t>(_toon_mode));

	stream.Write(_texture_add_factor);
	stream.Write(_texture_mul_factor);
	stream.Write(_sphere_add_factor);
	stream.Write(_sphere_mul_factor);
	stream.Write(_toon_add_factor);
	stream.Write(_toon_mul_factor);
}

void Material_MMD::LoadFromFile(FileStream& stream)
{
	uint8_t flag;
	
	stream.Read(flag); _draw_mode = static_cast<PMXDrawMode>(flag);
	stream.Read(flag); _sphere_op_mode = static_cast<PMXSphereMode>(flag);
	stream.Read(flag); _toon_mode = static_cast<PMXToonMode>(flag);

	stream.Read(_texture_add_factor);
	stream.Read(_texture_mul_factor);
	stream.Read(_sphere_add_factor);
	stream.Read(_sphere_mul_factor);
	stream.Read(_toon_add_factor);
	stream.Read(_toon_mul_factor);
}

void Material_Common::Clear()
{
	_diffuse = { 1, 1, 1, 1 };   // °¨¼â»ö
	_specular = { 1,1,1 };  // ±¤ÅÃ»ö
	_specularlity = 1;  // ±¤ÅÃµµ
	_ambient = { 1,1,1 };   // È¯°æ»ö
	
	_edge_color = { 1,1,1,1 };
	_edge_size = 1;
}

void Material_Common::SaveToFile(FileStream& stream) const
{
	stream.Write(_diffuse);
	stream.Write(_specular);
	stream.Write(_specularlity);
	stream.Write(_ambient);
	stream.Write(_edge_color);
	stream.Write(_edge_size);
}

void Material_Common::LoadFromFile(FileStream& stream)
{
	stream.Read(_diffuse);
	stream.Read(_specular);
	stream.Read(_specularlity);
	stream.Read(_ambient);
	stream.Read(_edge_color);
	stream.Read(_edge_size);
}

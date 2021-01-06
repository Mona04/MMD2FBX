#include "Framework.h"
#include "MMD_Importer.h"
#include "MMD/CustomMMDHelper.h"

#include "Core/DirectX/0_IADesc/Input_Desc.h"

#include "Core/Subsystem/Resource/ResourceManager.h"

#include "Resource/Mesh.h"
#include "Resource/SkeletalMesh.h"
#include "Resource/Skeletion.h"
#include "Resource/Material.h"
#include "Resource/Animation.h"

#include "Scene/Actor.h"
#include "Scene/Component/Renderable.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Animator.h"
#include "Scene/Component/IKSolver.h"

using namespace Framework;
using namespace pmx;


MMD_Importer::MMD_Importer()
{
}

MMD_Importer::~MMD_Importer()
{
	if (_fb) {
		_fb->close();
		SAFE_DELETE(_fb);
	}
	if (_stream)
		SAFE_DELETE(_stream);
}

bool MMD_Importer::Load_Model(std::wstring_view path, Actor* actor, Context* context)
{
	if (FileSystem::GetFileExtensionFromPath(path) != L".pmx")
	{
		LOG_ERROR("Invalid File form");
		return false;
	}
	if (FileSystem::IsExistFile(path) == false)
	{
		LOG_ERROR("NO File");
		return false;
	}

	_context = context;
	auto renderable = actor->GetComponent<Renderable>();
	auto transform = actor->GetComponent<Transform>();
	auto iksolver = actor->GetComponent<IKSolver>();
	if (!iksolver)
		iksolver = actor->AddComponent<IKSolver>();

	renderable->SetIsMMD(true);
	renderable->Clear();

	auto mgr = context->GetSubsystem<ResourceManager>();

	Init_PMX(path);

	LoadRenderable(renderable);
	LoadTransform(transform);
	iksolver->Init();

	return true;
}


bool MMD_Importer::Init_PMX(std::wstring_view path)
{
	_basePath = FileSystem::GetFileDirectoryFromPath(path);
	_basePathName = FileSystem::GetFileDirectoryFromPath(path) + FileSystem::GetIntactFileNameFromPath(path);

	_fb = new std::filebuf();
	if (!_fb->open(std::wstring(path).data(), std::ios::in | std::ios::binary))
	{
		LOG_WARNING("Can't open the " + FileSystem::ToString(path) + ", please check");
		return false;
	}

	_stream = new std::istream(_fb);
	if (!_stream)
	{
		LOG_WARNING("Failed to create IStream");
		return false;
	}

	char magic[4];
	_stream->read((char*)magic, sizeof(char) * 4);
	if (magic[0] != 0x50 || magic[1] != 0x4d || magic[2] != 0x58 || magic[3] != 0x20)
	{
		std::cerr << "invalid magic number." << std::endl;
		throw;
	}

	_stream->read((char*)&_version, sizeof(float));
	if (_version != 2.0f && _version != 2.1f)
	{
		std::cerr << "this is not ver2.0 or ver2.1 but " << _version << "." << std::endl;
		throw;
	}

	_setting.Read(_stream);

	this->_model_name = std::move(ReadString(_stream, _setting.encoding));
	this->_model_english_name = std::move(ReadString(_stream, _setting.encoding));
	this->_model_comment = std::move(ReadString(_stream, _setting.encoding));
	this->_model_english_comment = std::move(ReadString(_stream, _setting.encoding));

	return true;
}

bool MMD_Importer::LoadRenderable(Renderable* renderable)
{
	auto mgr = _context->GetSubsystem<ResourceManager>();

	{
		std::shared_ptr<SkeletalMesh> mesh = std::make_shared<SkeletalMesh>(_context);

		LoadVertices(mesh);
		LoadIndices(mesh);
		CalcTangent(mesh);

		mgr->RegisterResource(mesh, _basePathName + Extension_SkMeshW);
		renderable->AddMesh(mesh->GetPath());
	}

	{
		std::vector<std::wstring> texturesPath;
		LoadTexturePath(texturesPath);

		int material_count;
		_stream->read((char*)&material_count, sizeof(int));

		for (int i = 0; i < material_count; i++)
		{
			std::shared_ptr<Material> material = std::make_shared<Material>(_context);

			LoadMaterial(material, texturesPath);

			std::wstring myPath = _basePathName + std::to_wstring(i) + Extension_MaterialW;

			mgr->RegisterResource<Material>(material, myPath);
			renderable->AddMaterial(myPath);
		}
	}
	return true;
}

bool MMD_Importer::LoadVertices(std::shared_ptr<class SkeletalMesh> mesh)
{
	auto& vertices = mesh->GetVertices();
	int vertex_count = 0;
	_stream->read((char*)&vertex_count, sizeof(int));
	vertices.reserve(vertex_count);
	vertices.resize(vertex_count);

	for (int i = 0; i < vertex_count; ++i)
	{
		Vertex_MASTER& v = vertices[i];

		_stream->read((char*)(&v.pos), sizeof(float) * 3);
		_stream->read((char*)(&v.normal), sizeof(float) * 3);
		_stream->read((char*)(&v.uv), sizeof(float) * 2);

		PreProcessing_MMD_Vector3(v.pos, true);
		PreProcessing_MMD_Vector3(v.normal);

		assert(v.uv.x <= 1.0f);

		for (int set_uv = 0; set_uv < _setting.uv; ++set_uv)
		{
			_stream->read((char*)(&v.uva[set_uv]), sizeof(float) * 4);
		}

		PmxVertexSkinningType skinningType;
		_stream->read((char*)(&skinningType), sizeof(PmxVertexSkinningType));
		switch (skinningType)
		{
		case PmxVertexSkinningType::BDEF1:
			v.bone_index[0] = ReadIndex(_stream, _setting.bone_index_size);
			v.bone_weight[0] = 1.0f;
			break;
		case PmxVertexSkinningType::BDEF2:
			v.bone_index[0] = ReadIndex(_stream, _setting.bone_index_size);
			v.bone_index[1] = ReadIndex(_stream, _setting.bone_index_size);
			_stream->read((char*)&(v.bone_weight), sizeof(float));
			v.bone_weight[1] = 1 - v.bone_weight[0];
			break;
		case PmxVertexSkinningType::BDEF4:  // mostly used
			v.bone_index[0] = ReadIndex(_stream, _setting.bone_index_size);
			v.bone_index[1] = ReadIndex(_stream, _setting.bone_index_size);
			v.bone_index[2] = ReadIndex(_stream, _setting.bone_index_size);
			v.bone_index[3] = ReadIndex(_stream, _setting.bone_index_size);
			_stream->read((char*)&(v.bone_weight), sizeof(float) * 4);
			break;
		case PmxVertexSkinningType::SDEF:
			LOG_WARNING("I haven't learn SDEF. So I cannot handle it");
			v.bone_index[0] = ReadIndex(_stream, _setting.bone_index_size);
			v.bone_index[1] = ReadIndex(_stream, _setting.bone_index_size);
			// 뭔말인지 모르겠어서 제대로 구현 안함. 원래 코드 파서를 참고하자. 일단 캬루는 이거 안쓴다.
			_stream->read((char*)&(v.bone_weight), sizeof(float) * 3);
			_stream->read((char*)&(v.bone_weight), sizeof(float) * 3);
			_stream->read((char*)&(v.bone_weight), sizeof(float) * 3);
			_stream->read((char*)&(v.bone_weight), sizeof(float) * 1);
			break;
		case PmxVertexSkinningType::QDEF:
			v.bone_index[0] = ReadIndex(_stream, _setting.bone_index_size);
			v.bone_index[1] = ReadIndex(_stream, _setting.bone_index_size);
			v.bone_index[2] = ReadIndex(_stream, _setting.bone_index_size);
			v.bone_index[3] = ReadIndex(_stream, _setting.bone_index_size);
			_stream->read((char*)&(v.bone_weight), sizeof(float) * 4);
			break;
		default:
			LOG_WARNING("invalid skinning type");
		}

		_stream->read((char*)&(v.edge), sizeof(float));
	}

	return true;
}

bool MMD_Importer::LoadIndices(std::shared_ptr<class SkeletalMesh> mesh)
{
	auto& indices = mesh->GetIndices();
	int index_count = 0;
	_stream->read((char*)&index_count, sizeof(int));
	indices.reserve(index_count);
	indices.resize(index_count);

	for (int i = 0; i < index_count; i += 3)
	{
		indices[i + 0] = ReadIndex(_stream, _setting.vertex_index_size);
		indices[i + 1] = ReadIndex(_stream, _setting.vertex_index_size);
		indices[i + 2] = ReadIndex(_stream, _setting.vertex_index_size);
	}

	return true;
}

void MMD_Importer::CalcTangent(std::shared_ptr<class SkeletalMesh> mesh)
{
	auto& vertices = mesh->GetVertices();
	auto& indices = mesh->GetIndices();

	for (uint i = 0; i < indices.size() / 3; i++)
	{
		uint index0 = indices[i * 3 + 0];
		uint index1 = indices[i * 3 + 1];
		uint index2 = indices[i * 3 + 2];

		Vector3 p0 = vertices[index0].pos;
		Vector3 p1 = vertices[index1].pos;
		Vector3 p2 = vertices[index2].pos;

		Vector3 e0 = p1 - p0;
		Vector3 e1 = p2 - p0;

		Vector2 uv0 = vertices[index0].uv;
		Vector2 uv1 = vertices[index1].uv;
		Vector2 uv2 = vertices[index2].uv;

		float u0 = uv1.x - uv0.x;
		float u1 = uv2.x - uv0.x;

		float v0 = uv1.y - uv0.y;
		float v1 = uv2.y - uv0.y;

		float r = 1.0f / (u0 * v1 - v0 * u1);  // 역행렬용

		Vector3 normal = vertices[index0].normal;

		normal = Vector3::Cross(e1, e0).Normalize(); // adsf???

		Vector3 tangent;
		tangent.x = r * (v1 * e0.x - v0 * e1.x);
		tangent.y = r * (v1 * e0.y - v0 * e1.y);
		tangent.z = r * (v1 * e0.z - v0 * e1.z);
		if (isinf(r))
		{
			tangent = normal;
			tangent.x += 0.01f;
		}

		tangent = (tangent - normal * Vector3::Dot(tangent, normal)).Normalize();
		Vector3 binormal = Vector3::Cross(normal, tangent).Normalize();;

		vertices[index0].tangent = tangent;
		vertices[index1].tangent = tangent;
		vertices[index2].tangent = tangent;
		vertices[index0].binormal = binormal;
		vertices[index1].binormal = binormal;
		vertices[index2].binormal = binormal;
		vertices[index0].normal = normal;
		vertices[index1].normal = normal;
		vertices[index2].normal = normal;
	}
}

bool MMD_Importer::LoadTexturePath(std::vector<std::wstring>& texturesPath)
{
	int texture_count = 0;
	_stream->read((char*)&texture_count, sizeof(int));
	texturesPath.reserve(texture_count);
	texturesPath.resize(texture_count);

	for (int i = 0; i < texture_count; i++)
		texturesPath[i] = FileSystem::Find_Replace_All(ReadString(_stream, _setting.encoding), L"\\", L"/");

	return true;
}

bool MMD_Importer::LoadMaterial(std::shared_ptr<Material> material, const std::vector<std::wstring>& texturesPath)
{
	material->Set_MaterialName(ReadString(_stream, _setting.encoding));
	material->Set_MaterialEnglishName(ReadString(_stream, _setting.encoding));

	_stream->read((char*)&material->Get_DiffuseColor(), sizeof(float) * 4);
	_stream->read((char*)&material->Get_SpecularColor(), sizeof(float) * 3);
	_stream->read((char*)&material->Get_Specularity(), sizeof(float));
	_stream->read((char*)&material->Get_AmbientColor(), sizeof(float) * 3);
	_stream->read((char*)&material->Get_DrawMode(), sizeof(uint8_t));
	_stream->read((char*)&material->Get_EdgeColor(), sizeof(float) * 4);
	_stream->read((char*)&material->Get_EdgeSize(), sizeof(float));

	int diffuse_texture_index = ReadIndex(_stream, _setting.texture_index_size);
	int sphere_texture_index = ReadIndex(_stream, _setting.texture_index_size);
	int toon_texture_index = -1;
	_stream->read((char*)&material->Get_Sphere_op_mode(), sizeof(uint8_t));
	_stream->read((char*)&material->Get_Toon_Mode(), sizeof(uint8_t));

	if (material->Get_Toon_Mode() == PMXToonMode::Common)
	{
		_stream->read((char*)&toon_texture_index, sizeof(uint8_t));
	}
	else {
		toon_texture_index = ReadIndex(_stream, _setting.texture_index_size);
	}

	auto memo = std::move(ReadString(_stream, _setting.encoding));  // 이건 무시했음
	_stream->read((char*)&material->Get_IndexCount(), sizeof(int));

	if (diffuse_texture_index >= 0)
		material->Set_Texture(_basePath + texturesPath[diffuse_texture_index], Material::Type_Texture::Diffuse);
	if (sphere_texture_index >= 0)
		material->Set_Texture(_basePath + texturesPath[sphere_texture_index], Material::Type_Texture::Sphere);
	if (toon_texture_index >= 0)
		material->Set_Texture(_basePath + texturesPath[toon_texture_index], Material::Type_Texture::Toon);

	return true;
}
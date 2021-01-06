#include "Framework.h"
#include "Renderable.h"

#include "Resource/Mesh.h"
#include "Resource/SkeletalMesh.h"
#include "Resource/Material.h"
#include "Core/Subsystem/Resource/ResourceManager.h"

#include "Util/FileSystem.h"


using namespace Framework;

Renderable::Renderable(class Context* context) : IComponent(context)
{
	typecode = TypeCode::Renderable;
}

Renderable::~Renderable()
{
}

void Renderable::LoadFromFile(std::wstring_view path)
{
}

void Renderable::SaveToFile(std::wstring_view path)
{
	// material, mesh 각각 여기서 저장해야지, resourcemanager 소멸자에서 저장해봤자 무의미함.
}

bool Renderable::Update()
{
	return true;
}

void Renderable::Clear()
{
	_meshes.clear();
	_meshes.shrink_to_fit();
	_materials.clear();
	_materials.shrink_to_fit();
}


void Renderable::SetMeshes(std::vector<std::wstring_view>& paths)
{
	_meshes.clear();
	_meshes.shrink_to_fit();

	for (const auto& path : paths)
		AddMesh(path);
}

void Renderable::AddMesh()
{
	std::shared_ptr<Mesh> new_mesh = std::make_shared<Mesh>(_context);
	_meshes.push_back(new_mesh);
}

void Renderable::AddMesh(std::wstring_view path)
{
	std::shared_ptr<Mesh> result;
	auto extension = FileSystem::GetFileExtensionFromPath(path);
	if(extension ==  Extension_MeshW)
		result = _meshes.emplace_back(_resourceManager->GetResource<Mesh>(path));
	else if(extension == Extension_SkMeshW)
		result = _meshes.emplace_back(_resourceManager->GetResource<SkeletalMesh>(path));
	
	if (!result)
		LOG_WARNING("Invalid new Mesh. Check Path or File.");
}

void Renderable::DeleteMesh(uint i)
{
	auto iter = _meshes.begin();
	for (int ii = 0; ii < _meshes.size(); ii++, iter++)
	{
		if (ii = i)
		{
			_meshes.erase(iter);
			break;
		}
	}
}

void Renderable::SetMaterials(std::vector<std::wstring_view>& paths)
{
	_materials.clear();
	_materials.shrink_to_fit();

	for (const auto& path : paths)
		AddMaterial(path);
}

void Renderable::AddMaterial()
{
	std::shared_ptr<Material> new_material = std::make_shared<Material>(_context);
	_materials.push_back(new_material);
}

void Renderable::AddMaterial(std::wstring_view path)
{
	auto new_material = _resourceManager->GetResource<Material>(path);
	if (new_material)
		_materials.push_back(new_material);
	else
		LOG_WARNING("Invalid new Material. Check Path or File.");
}

void Renderable::DeleteMaterial(uint i)
{
	auto iter = _materials.begin();
	for (int ii = 0; ii < _materials.size(); ii++, iter++)
	{
		if (ii = i)
		{
			_materials.erase(iter);
			break;
		}
	}
}

void Renderable::SetRenderMesh()
{
	// RenderTarget 용 직사각형 만들려고
	_meshes.clear();
	_meshes.shrink_to_fit();
	AddMesh();

	_meshes[0]->MakeScreenMesh();
}

void Renderable::SetCube()
{
	// RenderTarget 용 직사각형 만들려고
	_meshes.clear();
	_meshes.shrink_to_fit();
	AddMesh();

	_meshes[0]->MakeCube();
}

void Renderable::SetGizmo()
{
	_meshes.clear();
	_meshes.shrink_to_fit();
	AddMesh();

	_meshes[0]->MakeGizmo();
}

void Renderable::SetGrid()
{
	_meshes.clear();
	_meshes.shrink_to_fit();
	AddMesh();

	_meshes[0]->MakeGrid();
}

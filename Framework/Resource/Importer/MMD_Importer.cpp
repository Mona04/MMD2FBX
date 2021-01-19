#include "Framework.h"
#include "MMD_Importer.h"
#include "MMD/CustomMMDHelper.h"

#include "Framework/Core/DirectX/0_IADesc/Input_Desc.h"

#include "Framework/Scene/Actor.h"
#include "Framework/Scene/Component/Renderable.h"
#include "Framework/Scene/Component/Transform.h"
#include "Framework/Scene/Component/Animator.h"
#include "Framework/Scene/Component/IKSolver.h"
#include "Framework/Scene/Component/RigidBody.h"

using namespace Framework;
using namespace pmx;

#define MMD_COMMON_DIRECTORYW (Relative_BasisW + L"_Assets/Texture/MMD/Common/")

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

void MMD_Importer::Clear()
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
		LOG_ERROR("Invalid Form");
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
	if (!iksolver) iksolver = actor->AddComponent<IKSolver>();
	auto rigidBodys = actor->GetComponent<RigidBodys>();
	if (!rigidBodys) rigidBodys = actor->AddComponent<RigidBodys>();
	
	renderable->SetIsMMD(true);
	renderable->Clear();

	Init_PMX(path);

	LoadRenderable(renderable);
	LoadTransform(transform);
	iksolver->Init();
	LoadMorph(renderable);
	LoadPhysics(rigidBodys);

	Clear();

	return true;
}


bool MMD_Importer::Init_PMX(std::wstring_view path)
{
	_basePath = FileSystem::GetFileDirectoryFromPath(path);
	_basePathName = FileSystem::GetFileDirectoryFromPath(path) + FileSystem::GetIntactFileNameFromPath(path);

	_fb = new std::filebuf();
	if (!_fb->open(path.data(), std::ios::in | std::ios::binary))
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
#pragma once
#include <memory>
#include <string>
#include <list>
#include "MMD/Pmx.h"
#include "Core/DirectX/0_IADesc/Input_Desc.h"

namespace Framework
{
	enum PMXBoneFlags : uint16_t
	{
		TargetShowMode = 0x0001,
		AllowRotate = 0x0002,
		AllowTranslate = 0x0004,
		Visible = 0x0008,
		AllowControl = 0x0010,
		IK = 0x0020,
		AppendLocal = 0x0080,
		AppendRotate = 0x0100,
		AppendTranslate = 0x0200,
		FixedAxis = 0x0400,
		LocalAxis = 0x800,
		DeformAfterPhysics = 0x1000,
		DeformOuterParent = 0x2000,
	};

	class MMD_Importer final
	{
	public:
		MMD_Importer();
		~MMD_Importer();

		bool Load_Model(std::wstring_view path, class Actor* actor, class Context* context);
		bool Load_Animation(std::wstring_view path, class Actor* actor, class Context* context);

	private:
		bool Init_PMX(std::wstring_view path);

		// =============== Renderable ================================= 
		bool LoadRenderable(class Renderable* renderable); 
		bool LoadVertices(std::shared_ptr<class SkeletalMesh> mesh);
		bool LoadIndices(std::shared_ptr<class SkeletalMesh> mesh);
		void CalcTangent(std::shared_ptr<class SkeletalMesh> mesh);

		bool LoadTexturePath(std::vector<std::wstring>& texturePaths);
		bool LoadMaterial(std::shared_ptr<class Material> material, const std::vector<std::wstring>& texturePaths);
		// ============================================================

		// =============== Transform ==================================
		bool LoadTransform(class Transform* transform);
		bool LoadSkeleton(std::shared_ptr<class Skeleton> skeleton, std::vector<std::pair<int, int>>& bone_links);
		bool LoadSkeleton_Resursive( std::shared_ptr<class Skeleton> skeletons, std::vector<std::pair<int, int>>& bone_links);

		bool LoadMorph(std::shared_ptr<class Skeleton> skeletons);
		// =============== Transform =================================


		// =============== Animation =================================
		bool Init_VMD(std::wstring_view path);
		bool LoadBoneFrame(std::shared_ptr<class Animation> animation, std::shared_ptr<class Skeleton> skeleton);
		void CalcBezier(Vector2* desc, const uint8_t* src);

		bool LoadFaceFrame(std::shared_ptr<class Animation> animation, std::shared_ptr<class Skeleton> skeleton);
		bool LoadCameraFrame(std::shared_ptr<class Animation> animation, std::shared_ptr<class Skeleton> skeleton);
		bool LoadLightFrame(std::shared_ptr<class Animation> animation, std::shared_ptr<class Skeleton> skeleton);
		bool LoadIKFrame(std::shared_ptr<class Animation> animation, std::shared_ptr<class Skeleton> skeleton);

		// ===========================================================
	private:
		class Context* _context;

		float _version;
		std::wstring _basePath;
		std::wstring _basePathName;

		std::filebuf* _fb;
		std::istream* _stream;

		pmx::PmxSetting  _setting;
		std::wstring _model_name;
		std::wstring _model_english_name;
		std::wstring _model_comment;
		std::wstring _model_english_comment;
	};

	class VmdIkEnable
	{
	public:
		std::wstring ik_name;
		bool enable;
	};
}
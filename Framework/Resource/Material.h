#pragma once
#include <vector>
#include <cstddef>
#include <memory>
#include <unordered_map>
#include "IResource.h"

namespace Framework
{
	/*
	0x01:양면묘화
	0x02:지면그림자
	0x04:셀프 쉐도우맵에 묘화
	0x08:셀프 쉐도우 묘화
	0x10:엣지 묘화
	0x20:정점 색상 ( 2.1 확장 )
	0x40:포인트 묘화 ( 2.1 확장 )
	0x80:라인 묘화 ( 2.1 확장)
	*/
	enum PMXDrawMode : uint8_t
	{
		BothFace = 0x01,
		GroundShadow = 0x02,
		CastSelfShadow = 0x04,
		RecieveSelfShadow = 0x08,
		DrawEdge = 0x10,
		VertexColor = 0x20,
		DrawPoint = 0x40,
		DrawLine = 0x80,
	};

	enum class PMXSphereMode: uint8_t
	{
		None, // 무효
		Mul,  // 곱
		Add,
		SubTexture,  // 추가 uv1 의 x,y 를 참조해서 통상 텍스쳐 그리기 진행
	};

	enum class PMXToonMode : uint8_t
	{
		Separate,	//!< 0:개별 툰
		Common,		//!< 1:공유 툰 Toon[0-9] toon01.bmp～toon10.bmp
	};

	class Material : public IResource, public std::enable_shared_from_this<Material>
	{
	public:
		enum class Type_Texture : uint {
			None, 
			Diffuse, Specular, Ambient, Emissive, Height, Normal, Shininess, Opacity, Displacement, 
			Lightmap, Reflection, BaseColor, NormalCamera, EmissionColor, 
			Metalness, DiffuseRoughness, AbientOcclusion,
			Sphere, Toon  // MMD
		};

	public:
		Material(Context* context);
		virtual ~Material();

		Material(Material& rhs) = delete;
		Material(Material&& rhs) = delete;
		Material& operator=(Material& rhs) = delete;
		Material& operator=(Material&& rhs) = delete;

		virtual bool LoadFromFile(std::wstring_view path) override;
		virtual bool SaveToFile(std::wstring_view path) override;
		virtual void Clear() override;

	private:
		virtual bool CreateBuffer();

	public:
		void Set_MaterialName(const std::wstring& var) { _material_name = var; }
		std::wstring& Get_MaterialName() { return _material_name; }
		void Set_MaterialEnglishName(const std::wstring& var) { _material_english_name = var; }
		std::wstring& Get_MaterialEnglishName() { return _material_english_name; }

		void Set_Texture(std::wstring_view path, Type_Texture type);
		std::wstring Get_TexturePath(Type_Texture type);
		std::shared_ptr<class Texture> Get_Texture(Type_Texture type);


		void Set_DiffuseColor(Color4 var) { _diffuse = var; }
		Color4& Get_DiffuseColor() { return _diffuse; }
		void Set_SpecularColor(Vector3 var) { _specular = var; }
		Vector3& Get_SpecularColor() { return _specular; }
		void Set_Specularity(float var) { _specularlity = var; }
		float& Get_Specularity() { return _specularlity; }
		
		void Set_AmbientColor(Vector3 var) { _ambient = var; }
		Vector3& Get_AmbientColor() { return _ambient; }

		void Set_EdgeColor(Color4 var) { _edge_color = var; }
		Color4& Get_EdgeColor() { return _edge_color; }
		void Set_EdgeSize(float var) { _edge_size = var; }
		float& Get_EdgeSize() { return _edge_size; }

		void Set_DrawMode(PMXDrawMode var) { _draw_mode = var; }
		PMXDrawMode& Get_DrawMode() { return _draw_mode; }
		void Set_Sphere_op_mode(PMXSphereMode var) { _sphere_op_mode = var; }
		PMXSphereMode& Get_Sphere_op_mode() { return _sphere_op_mode; }
		void Set_Toon_Mode(PMXToonMode var) { _toon_mode = var; }
		PMXToonMode& Get_Toon_Mode() { return _toon_mode; }

		void Set_IndexCount(uint var) { _index_count = var; }
		uint& Get_IndexCount() { return _index_count; }

	private:	 
		//std::vector<class Texture> _textures;
		std::wstring _material_name = L"";
		std::wstring _material_english_name = L"";

		std::unordered_map<Type_Texture, std::shared_ptr<class Texture>> _textures;

		Color4 _diffuse = { 1, 1, 1, 1 };   // 감쇄색
		Vector3 _specular = { 1,1,1 };  // 광택색
		float _specularlity = 1;  // 광택도
		Vector3 _ambient = { 1,1,1 };   // 환경색

		Color4 _edge_color = { 1,1,1,1 };
		float _edge_size = 1;

		PMXDrawMode _draw_mode = PMXDrawMode::VertexColor;  // 묘화 플래그
		PMXSphereMode _sphere_op_mode = PMXSphereMode::Add;   // 스피어 텍스쳐 연산모드
		PMXToonMode _toon_mode= PMXToonMode::Common;   // // 공유툰 플래그

		uint _index_count = -1;  // multiple material 시 사용
	};
}
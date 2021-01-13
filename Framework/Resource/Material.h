#pragma once
#include <vector>
#include <cstddef>
#include <memory>
#include <unordered_map>
#include "IResource.h"

namespace Framework
{
	/*
	0x01:��鹦ȭ
	0x02:����׸���
	0x04:���� ������ʿ� ��ȭ
	0x08:���� ������ ��ȭ
	0x10:���� ��ȭ
	0x20:���� ���� ( 2.1 Ȯ�� )
	0x40:����Ʈ ��ȭ ( 2.1 Ȯ�� )
	0x80:���� ��ȭ ( 2.1 Ȯ��)
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

	enum class PMXSphereMode : uint8_t
	{
		None, // ��ȿ
		Mul,  // ��
		Add,
		SubTexture,  // �߰� uv1 �� x,y �� �����ؼ� ��� �ؽ��� �׸��� ����
	};

	enum class PMXToonMode : uint8_t
	{
		Separate,	//!< 0:���� ��
		Common,		//!< 1:���� �� Toon[0-9] toon01.bmp��toon10.bmp
	};

	struct Material_MMD
	{
		void Clear();
		void SaveToFile(FileStream& stream);
		void LoadFromFile(FileStream& stream);

		PMXDrawMode _draw_mode = PMXDrawMode::VertexColor;  // ��ȭ �÷���
		PMXSphereMode _sphere_op_mode = PMXSphereMode::Add;   // ���Ǿ� �ؽ��� ������
		PMXToonMode _toon_mode = PMXToonMode::Common;   // // ������ �÷���

		Color4 _texture_add_factor = { 0, 0, 0, 0 };
		Color4 _texture_mul_factor = { 1, 1, 1, 1 };
		Color4 _sphere_add_factor = { 0, 0, 0, 0 };
		Color4 _sphere_mul_factor = { 1, 1, 1, 1 };
		Color4 _toon_add_factor = { 0, 0, 0, 0 };
		Color4 _toon_mul_factor = { 1, 1, 1, 1 };
	};

	struct Material_Common
	{
		void Clear();
		void SaveToFile(FileStream& stream);
		void LoadFromFile(FileStream& stream);

		Color4 _diffuse = { 1, 1, 1, 1 };   // �����
		Vector3 _specular = { 1,1,1 };  // ���û�
		float _specularlity = 1;  // ���õ�
		Vector3 _ambient = { 1,1,1 };   // ȯ���

		Color4 _edge_color = { 1,1,1,1 };
		float _edge_size = 1;
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

	public:
		void Set_MaterialName(const std::wstring& var) { _material_name = var; }
		std::wstring& Get_MaterialName() { return _material_name; }
		void Set_MaterialEnglishName(const std::wstring& var) { _material_english_name = var; }
		std::wstring& Get_MaterialEnglishName() { return _material_english_name; }

		void Set_Texture(std::wstring_view path, Type_Texture type);
		std::wstring Get_TexturePath(Type_Texture type);
		std::shared_ptr<class Texture> Get_Texture(Type_Texture type);

		Material_MMD& Get_Material_MMD() { return _material_mmd; }
		Material_Common& Get_Material_Common() { return _material_common; }

		void Set_IndexCount(uint var) { _index_count = var; }
		uint& Get_IndexCount() { return _index_count; }

	private:
		std::wstring _material_name = L"";
		std::wstring _material_english_name = L"";

		std::unordered_map<Type_Texture, std::shared_ptr<class Texture>> _textures;

		Material_MMD _material_mmd;
		Material_Common _material_common;

		uint _index_count = -1;  // multiple material �� ���
	};
}